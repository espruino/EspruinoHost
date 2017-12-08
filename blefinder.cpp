#include "app.h"
#include "blefinder.h"
#include <QMetaEnum>

BleFinder::BleFinder(QObject *parent) :
    QObject(parent),
    m_deviceDiscoveryAgent(nullptr),
    m_scanTimer(nullptr)
{
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000); // FIXME? 0 => always keep going

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BleFinder::addDevice);
    connect(m_deviceDiscoveryAgent, static_cast<void (QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error)>(&QBluetoothDeviceDiscoveryAgent::error),
            this, &BleFinder::scanError);

    m_scanTimer.setSingleShot(true);
    m_scanTimer.setInterval(5000);
    connect(&m_scanTimer, &QTimer::timeout, this, &BleFinder::handleDiscoveryTimeout);
    //    m_scanTimer.start();

    // TODO: Remove old devices
}

BleFinder::~BleFinder()
{
    m_devices.clear();
}

QList<QBluetoothDeviceInfo> BleFinder::getDevices()
{
    if (!m_deviceDiscoveryAgent->isActive()) {
        g_app->log("BLE: Starting scan");
        m_devices.clear();
        m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    }
    // if the timer is running, it'll be restarted
    m_scanTimer.start();

    return m_devices;
}

void BleFinder::forceStop()
{
    g_app->log("BLE: Force stop scan");
    m_scanTimer.stop();
    m_deviceDiscoveryAgent->stop();
}

const QBluetoothDeviceInfo *BleFinder::findDeviceByAddress(QString address)
{
    const QBluetoothDeviceInfo *device = 0;
    for (int i = 0; i < m_devices.size(); i++) {
        if (getDeviceAddress(m_devices.at(i)) == address ) {
            device = &m_devices.at(i);
            break;
        }
    }
    return device;
}

QString BleFinder::getDeviceAddress(const QBluetoothDeviceInfo &device) {
#if defined Q_OS_DARWIN
    // workaround for Core Bluetooth:
    return device.deviceUuid().toString();
#else
    return device.address().toString();
#endif
}

// ==================================================================================================================

void BleFinder::addDevice(const QBluetoothDeviceInfo &device)
{
    // If device is LowEnergy-device, add it to the list
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        m_devices.append(device);
        g_app->log("BLE: device found:" + device.name());
    }
}

void BleFinder::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QBluetoothDeviceDiscoveryAgent::Error>();
    g_app->error(QString("SCAN ERROR ") + metaEnum.valueToKey(error));
}

void BleFinder::handleDiscoveryTimeout()
{
    g_app->log("BLE: Stopping scan after inactivity");
    m_deviceDiscoveryAgent->stop();
}
