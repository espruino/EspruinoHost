#include "websocketclient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QtCore/QDebug>
#include "QtWebSockets/QWebSocket"

/**

    QJsonObject json;
    json["type"]=QJsonValue("write");
    QJsonDocument jsondoc(json);
    QString::fromUtf8(saveDoc.toJson())

  */


WebSocketClient::WebSocketClient(QObject *parent) :
    QObject(parent),
    pWebSocket(nullptr)
{

}

WebSocketClient::~WebSocketClient() {
  if (pWebSocket) pWebSocket->deleteLater();
  pWebSocket = 0;
}

void WebSocketClient::receive(QString message) // data received from remote computer
{
    QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();
    qDebug() << "Text JSON " << json["type"];
}

void WebSocketClient::send(QString message) // send to remote computer
{
  if (pWebSocket)
      pWebSocket->sendTextMessage(message);
}
