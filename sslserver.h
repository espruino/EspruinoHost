#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include <QAbstractSocket>
#include <QWebSocketProtocol>
#include <QSslPreSharedKeyAuthenticator>
#include "app.h"
#include "websocketclient.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class SslServer : public QObject
{
    Q_OBJECT
public:
    explicit SslServer(QObject *parent = nullptr);
    ~SslServer();

private:


private Q_SLOTS:
    void onAcceptError(QAbstractSocket::SocketError socketError);
    void onPeerVerifyError(QSslError socketError);
    void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *authenticator);
    void onServerError(QWebSocketProtocol::CloseCode closeCode);
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

    WebSocketClient *findClientForSocket(QWebSocket* socket);
signals:

public slots:

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<WebSocketClient *> m_clients;
};

#endif // SSLSERVER_H
