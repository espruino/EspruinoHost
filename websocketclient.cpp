#include "websocketclient.h"
#include "app.h"
#include "blefinder.h"
#include "bleuart.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSerialPortInfo>
#include <QtSerialPort/QtSerialPort>
#include "QtWebSockets/QWebSocket"

WebSocketClient::WebSocketClient(QObject *parent) :
    QObject(parent),
    pWebSocket(nullptr),
    m_connectionType(CT_DISCONNECTED),
    m_bytesWriting(0),
    m_ifSerialPort(this),
    m_ifBluetooth(this)
{
    connect(&m_ifSerialPort, &QSerialPort::readyRead, this, &WebSocketClient::handleSerialReadyRead);
    connect(&m_ifSerialPort, &QSerialPort::bytesWritten, this, &WebSocketClient::handleSerialBytesWritten);
    connect(&m_ifSerialPort, &QSerialPort::errorOccurred, this, &WebSocketClient::handleSerialError);

    connect(&m_ifBluetooth, &BleUART::connectionChanged, this, &WebSocketClient::handleBluetoothConnection);
    connect(&m_ifBluetooth, &BleUART::dataReceived, this, &WebSocketClient::handleBluetoothData);

}

WebSocketClient::~WebSocketClient() {
  if (pWebSocket) pWebSocket->deleteLater();
  pWebSocket = 0;
}

void WebSocketClient::receive(QString message) // data received from remote computer
{
    QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();
    if (json["type"].isNull()) {
        g_app->warn("No JSON 'type' - ignoring packet");
        return;
    }
    QString jType = json["type"].toString();
    g_app->log("JSON type " + jType);
    if (jType=="version") {
        QJsonObject json;
        json["type"] = QJsonValue("version");
        json["version"] = QJsonValue(APP_VERSION);
        send(json);
    } else if (jType=="list") {
        QJsonObject json;
        json["type"] = QJsonValue("list");
        QJsonArray jsPortList;
        // Serial
        QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
        for (int i=0; i<serialPorts.size();i++) {
            QSerialPortInfo serialPort = serialPorts.at(i);
            QJsonObject jsPort;
            jsPort["interface"] = QJsonValue("serial");
            jsPort["description"] = QJsonValue(serialPort.description());
            jsPort["path"] = QJsonValue(serialPort.systemLocation());
            jsPortList.append(jsPort);
        }
        // Bluetooth
        QList<QBluetoothDeviceInfo> bleDevices = g_app->m_bleFinder->getDevices();
        for (int i=0; i<bleDevices.size();i++) {
            const QBluetoothDeviceInfo &bleDevice = bleDevices.at(i);
            if (bleDevice.isCached() || !bleDevice.isValid()) continue;
            QJsonObject jsPort;
            jsPort["interface"] = QJsonValue("bluetooth");
            jsPort["description"] = QJsonValue(bleDevice.name());
            jsPort["rssi"] = QJsonValue(bleDevice.rssi());
            jsPort["path"] = QJsonValue(g_app->m_bleFinder->getDeviceAddress(bleDevice));
            jsPortList.append(jsPort);
        }
        // Send response
        json["ports"] = jsPortList;
        send(json);
    } else if (jType=="connect") {
        if (m_connectionType!=CT_DISCONNECTED) {
            g_app->warn("Already connected");
            return;
        }

        QString jInterface = json["interface"].toString();
        QString jPath = json["path"].toString();
        if (jInterface=="serial") {
            m_ifSerialPort.setPortName(jPath);
            int baud = 9600;
            if (!json["baud"].isNull())
                baud = json["baud"].toInt();
            m_ifSerialPort.setBaudRate(baud);
            if (m_ifSerialPort.open(QIODevice::ReadWrite)) {
                m_connectionType=CT_SERIALPORT;
                send("{\"type\":\"connect\"}");
            } else {
                QJsonObject json;
                json["type"] = QJsonValue("error");
                QMetaEnum metaEnum = QMetaEnum::fromType<QSerialPort::SerialPortError>();
                json["message"] = QJsonValue(metaEnum.valueToKey(m_ifSerialPort.error()));
                send(json);
            }
        } else if (jInterface=="bluetooth") {
            const QBluetoothDeviceInfo *device = g_app->m_bleFinder->findDeviceByAddress(jPath);
            if (device) {
                g_app->m_bleFinder->forceStop();
                m_ifBluetooth.connectToDevice(*device);
                send("{\"type\":\"connect\"}");
                m_connectionType=CT_BLUETOOTH;
            } else {
                QJsonObject json;
                json["type"] = QJsonValue("error");
                json["message"] = QJsonValue("Device not found");
                send(json);
            }
        } else{
            g_app->warn("Unknown interface " + jInterface + " for 'connect'");
        }
    } else if (jType=="disconnect") {
        if (m_connectionType==CT_SERIALPORT) {
            m_ifSerialPort.close();
            m_connectionType = CT_DISCONNECTED;
        } else if (m_connectionType==CT_BLUETOOTH) {
            m_ifBluetooth.disconnect();
            m_connectionType = CT_DISCONNECTED;
        } else {
            g_app->warn("Not connected for disconnect");
        }
    } else if (jType=="write") {
        QString jData = json["data"].toString();
        QByteArray arr = jData.toUtf8();
        m_bytesWriting += arr.length();
        if (m_connectionType==CT_SERIALPORT) {
            qint64 bytesWritten = m_ifSerialPort.write(arr);
            // FIXME: compare written vs writing byte count
        } else if (m_connectionType==CT_BLUETOOTH) {
            m_ifBluetooth.write(arr);
        } else {
            g_app->warn("Not connected for write");
        }
    } else {
        g_app->warn( "Unknown command type " + jType);
    }
}

void WebSocketClient::send(QString message) // send to remote computer
{
  if (pWebSocket)
      pWebSocket->sendTextMessage(message);
}

void WebSocketClient::send(QJsonObject json) // send to remote computer
{
  QJsonDocument jsondoc(json);
  send(QString::fromUtf8(jsondoc.toJson()));
}

// ============================================================================== SERIAL


void WebSocketClient::handleSerialReadyRead()
{
    QJsonObject json;
    json["type"] = QJsonValue("read");
    json["data"] = QJsonValue(QString::fromUtf8(m_ifSerialPort.readAll()));
    send(json);
}

void WebSocketClient::handleSerialBytesWritten(qint64 bytes)
{
    m_bytesWriting -= bytes;
    if (m_bytesWriting<=0) {
        QJsonObject json;
        json["type"] = QJsonValue("write");
        json["count"] = QJsonValue(bytes);
        send(json);
    }
}

void WebSocketClient::handleSerialError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::SerialPortError::NoError) return; // ???
    QJsonObject json;
    json["type"] = QJsonValue("error");
    QMetaEnum metaEnum = QMetaEnum::fromType<QSerialPort::SerialPortError>();
    json["message"] = QJsonValue(metaEnum.valueToKey(serialPortError));
    send(json);
    g_app->error(QString("Serial ") + metaEnum.valueToKey(serialPortError)+ " occurred on "+m_ifSerialPort.portName()+" - "+m_ifSerialPort.errorString());
}

// ============================================================================== BLUETOOTH

void WebSocketClient::handleBluetoothConnection() {

}

void WebSocketClient::handleBluetoothData(QByteArray data) {
    QJsonObject json;
    json["type"] = QJsonValue("read");
    json["data"] = QJsonValue(QString::fromUtf8(data));
    send(json);
}
