#ifndef BLEFINDER_H
#define BLEFINDER_H

#include <QObject>
#include <QTimer>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

class BleFinder : public QObject
{
    Q_OBJECT
public:
    explicit BleFinder(QObject *parent = nullptr);
    ~BleFinder();

    QList<QBluetoothDeviceInfo> getDevices();
    void forceStop();
    const QBluetoothDeviceInfo *findDeviceByAddress(QString address);

    QString getDeviceAddress(const QBluetoothDeviceInfo &device);

signals:

private slots:
    void addDevice(const QBluetoothDeviceInfo&);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void handleDiscoveryTimeout();

public slots:

private:
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QList<QBluetoothDeviceInfo> m_devices;
    QTimer m_scanTimer;
};

#endif // BLEFINDER_H
