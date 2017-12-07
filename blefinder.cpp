#include "blefinder.h"
#include <QtCore/QDebug>
#include <QMetaEnum>

BleFinder::BleFinder(QObject *parent) :
    QObject(parent),
    m_deviceDiscoveryAgent(nullptr),
    m_scanTimer(nullptr)
{
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(0); // always keep going

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BleFinder::addDevice);
    connect(m_deviceDiscoveryAgent, static_cast<void (QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error)>(&QBluetoothDeviceDiscoveryAgent::error),
            this, &BleFinder::scanError);

    m_scanTimer.setSingleShot(true);
    m_scanTimer.setInterval(2000);
    connect(&m_scanTimer, &QTimer::timeout, this, &BleFinder::handleDiscoveryTimeout);
    //    m_scanTimer.start();

    // TODO: Remove old devices
}

BleFinder::~BleFinder()
{
    // qDeleteAll(m_devices); // we don't own these devices
    m_devices.clear();
}

QList<QBluetoothDeviceInfo> BleFinder::getDevices()
{
    if (!m_deviceDiscoveryAgent->isActive()) {
        qDebug() << "Starting scan";
        m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    }
    // if the timer is running, it'll be restarted
    m_scanTimer.start();

    return m_devices;
}

void BleFinder::forceStop()
{
    qDebug() << "BLE: Force stopping scan";
    m_scanTimer.stop();
    m_deviceDiscoveryAgent->stop();
}

void BleFinder::addDevice(const QBluetoothDeviceInfo &device)
{
    // If device is LowEnergy-device, add it to the list
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        m_devices.append(device);
        qDebug() << "BLE: device found:" << device.name();
    }
}

void BleFinder::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QBluetoothDeviceDiscoveryAgent::Error>();
    qDebug() << "SCAN ERROR " << metaEnum.valueToKey(error);
}

void BleFinder::handleDiscoveryTimeout()
{
    qDebug() << "BLE: Stopping scan after inactivity";
    m_deviceDiscoveryAgent->stop();
}