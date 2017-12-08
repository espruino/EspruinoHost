#include "sslserver.h"
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include "app.h"
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

#include "websocketclient.h"

SslServer::SslServer(QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(nullptr)
{
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("SSL Espruino Host"),
                                              QWebSocketServer::SecureMode,
                                              this);
    QSslConfiguration sslConfiguration;
    QFile certFile(QStringLiteral(":/keys/localhost.cert"));
    QFile keyFile(QStringLiteral(":/keys/localhost.key"));
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
    m_pWebSocketServer->setSslConfiguration(sslConfiguration);

    int port = WEBSOCKET_PORT;

    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        g_app->status(QString("WebSocket listening on wss://localhost:") + QString::number(port));
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &SslServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::sslErrors,
                this, &SslServer::onSslErrors);
    }
}

SslServer::~SslServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
    m_clients.clear();
}

void SslServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    WebSocketClient *wsClient = new WebSocketClient(nullptr);
    wsClient->pWebSocket = pSocket;

    g_app->log(QString("WebSocket Client connected:") + pSocket->peerName() + " " + pSocket->origin());

    connect(pSocket, &QWebSocket::textMessageReceived, this, &SslServer::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &SslServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &SslServer::socketDisconnected);

    m_clients << wsClient;
    g_app->status(QString::number(m_clients.length()) + QString(" clients connected"));
}

void SslServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    WebSocketClient *wsClient = SslServer::findClientForSocket(pClient);
    if (wsClient) wsClient->receive(message);
}

void SslServer::processBinaryMessage(QByteArray message)
{
    g_app->warn("Binary WebSocket message not handled");
}

void SslServer::socketDisconnected()
{
    g_app->log("WebSocket Client disconnected");
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    WebSocketClient *wsClient = SslServer::findClientForSocket(pClient);
    if (wsClient) {
        m_clients.removeAll(wsClient);
        wsClient->deleteLater();
    }
    g_app->status(QString::number(m_clients.length()) + QString(" clients connected"));
}

void SslServer::onSslErrors(const QList<QSslError> &)
{
    g_app->error("SSL errors occurred");
}

WebSocketClient *SslServer::findClientForSocket(QWebSocket* socket)
{
    for (int i = 0; i < m_clients.size(); ++i)
        if (m_clients.at(i)->pWebSocket == socket)
            return m_clients.at(i);
    return nullptr;
}
