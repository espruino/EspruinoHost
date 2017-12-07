#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include "websocketclient.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(MainWindow)

class SslServer : public QObject
{
    Q_OBJECT
public:
    explicit SslServer(QObject *parent = nullptr, MainWindow *mainWindow = nullptr);
    ~SslServer();

private:
    MainWindow *m_pMainWindow;

private Q_SLOTS:
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
