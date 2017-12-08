#ifndef BLEUART_H
#define BLEUART_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>

class BleUART : public QObject
{
    Q_OBJECT
public:
    explicit BleUART(QObject *parent = nullptr);
    void connectToDevice(QBluetoothDeviceInfo device);
    ~BleUART();

    void write(QByteArray data);
    void disconnect();

private:
    bool m_foundUARTService;
    QLowEnergyController *m_control;
    QLowEnergyService *m_service;
    QLowEnergyCharacteristic m_txChar;
    QLowEnergyCharacteristic m_rxChar;
    QLowEnergyDescriptor m_rxDesc;

    QByteArray bleWriteQueue;

    void handleConnected();
    void handleDisconnected();
    void handleError(QLowEnergyController::Error error);

    //QLowEnergyController
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanDone();

    //QLowEnergyService
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void updateCharacteristicValue(const QLowEnergyCharacteristic &c,
                                   const QByteArray &value);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                                  const QByteArray &value);
    void confirmedCharacteristicWrite(const QLowEnergyCharacteristic &d,
                                      const QByteArray &value);
signals:
    void connectionChanged(bool connected);
    void dataReceived(QByteArray data);
    void dataWritten();

public slots:
};

#endif // BLEUART_H
