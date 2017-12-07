#include "bleuart.h"

#define NORDIC_SERVICE QString("6e400001-b5a3-f393-e0a9-e50e24dcca9e")
#define NORDIC_TX QString("6e400002-b5a3-f393-e0a9-e50e24dcca9e")
#define NORDIC_RX QString("6e400003-b5a3-f393-e0a9-e50e24dcca9e")




BleUART::BleUART(QObject *parent) :
    QObject(parent)
{
    m_foundUARTService = false;
    m_control = 0;
    m_service = 0;
}

void BleUART::connectToDevice(QBluetoothDeviceInfo device)
{
    m_control = new QLowEnergyController(device, this);
    m_control->setRemoteAddressType(QLowEnergyController::RandomAddress); // FIXME (or Public)
    connect(m_control, &QLowEnergyController::serviceDiscovered,
            this, &BleUART::serviceDiscovered);
    connect(m_control, &QLowEnergyController::discoveryFinished,
            this, &BleUART::serviceScanDone);

    connect(m_control, &QLowEnergyController::connected, this, &BleUART::handleConnected);
    connect(m_control, &QLowEnergyController::disconnected, this, &BleUART::handleDisconnected);
    connect(m_control, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error),
            this, &BleUART::handleError);

    // Connect
    m_control->connectToDevice();
}

BleUART::~BleUART() {
    m_control->disconnectFromDevice();
    delete m_control;
}

void BleUART::handleConnected()
{
    qDebug() << "Controller connected. Search services...";
    m_control->discoverServices();
}

void BleUART::handleDisconnected()
{
    qDebug() << "LowEnergy controller disconnected";
}

void BleUART::handleError(QLowEnergyController::Error error)
{
    Q_UNUSED(error);
    qDebug() << "Cannot connect to remote device.";
}

void BleUART::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if (gatt == QBluetoothUuid(NORDIC_SERVICE)) {
        m_foundUARTService = true;
        qDebug() << "UART Service found.";
    }
}

void BleUART::serviceScanDone()
{
    qDebug() << "Service scan done.";

    // Delete old service if available
    if (m_service) {
        delete m_service;
        m_service = 0;
    }

    if (m_foundUARTService)
        m_service = m_control->createServiceObject(QBluetoothUuid(NORDIC_SERVICE), this);

    if (m_service) {
        connect(m_service, &QLowEnergyService::stateChanged, this, &BleUART::serviceStateChanged);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &BleUART::updateCharacteristicValue);
        connect(m_service, &QLowEnergyService::descriptorWritten, this, &BleUART::confirmedDescriptorWrite);
        m_service->discoverDetails();
    } else {
        qDebug() << "UART Service not found.";
    }
}

void BleUART::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::DiscoveringServices:
        qDebug() << "Discovering services...";
        break;
    case QLowEnergyService::ServiceDiscovered:
    {
        qDebug() << "Service discovered.";

        m_txChar = m_service->characteristic(QBluetoothUuid(NORDIC_TX));
        if (!m_txChar.isValid()) {
            qDebug() << "TX Characteristic not found.";
        }
        m_rxChar = m_service->characteristic(QBluetoothUuid(NORDIC_RX));
        if (!m_rxChar.isValid()) {
            qDebug() << "RX Characteristic not found.";
            break;
        }

        m_rxDesc = m_rxChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
        if (m_rxDesc.isValid())
            m_service->writeDescriptor(m_rxDesc, QByteArray::fromHex("0100"));

        break;
    }
    default:
        //nothing for now
        break;
    }

    emit connectionChanged();
}

void BleUART::updateCharacteristicValue(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "Characteristic changed" << QString::fromUtf8(value);

    if (c.uuid() != QBluetoothUuid(NORDIC_RX))
        return;

    //const quint8 *data = reinterpret_cast<const quint8 *>(value.constData());
    emit dataReceived(value);
}

void BleUART::confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    if (d.isValid() && d == m_rxDesc && value == QByteArray::fromHex("0000")) {
        //disabled notifications -> assume disconnect intent
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = 0;
    }
}

void BleUART::write(QByteArray data) {
    if (m_txChar.isValid()) {
        m_service->writeCharacteristic(m_txChar, data, QLowEnergyService::WriteWithoutResponse);
    }
}

void BleUART::disconnect()
{
    m_foundUARTService = false;

    //disable notifications
    if (m_rxDesc.isValid() && m_service
            && m_rxDesc.value() == QByteArray::fromHex("0100")) {
        m_service->writeDescriptor(m_rxDesc, QByteArray::fromHex("0000"));
    } else {
        if (m_control)
            m_control->disconnectFromDevice();

        delete m_service;
        m_service = 0;
    }
}
