#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QSerialPort>
#include "bleuart.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QJsonObject)
QT_FORWARD_DECLARE_CLASS(MainWindow)

class WebSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    void receive(QString message); // data received from remote computer
    void send(QString message); // send to remote computer
    void send(QJsonObject json); // send to remote computer

    QWebSocket *pWebSocket;

private:
    enum { CT_DISCONNECTED, CT_SERIALPORT, CT_BLUETOOTH } m_connectionType;
    int m_bytesWriting;
    // Interfaces
    QSerialPort m_ifSerialPort;
    BleUART m_ifBluetooth;

private slots:
    void handleSerialReadyRead();
    void handleSerialBytesWritten(qint64 bytes);
    void handleSerialError(QSerialPort::SerialPortError error);

    void handleBluetoothConnection(bool connected);
    void handleBluetoothData(QByteArray data);
    void handleBluetoothWritten();

signals:

public slots:
};

#endif // WEBSOCKETCLIENT_H
