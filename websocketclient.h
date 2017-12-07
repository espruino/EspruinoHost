#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    void receive(QString message); // data received from remote computer
    void send(QString message); // send to remote computer

    QWebSocket *pWebSocket;
signals:

public slots:
};

#endif // WEBSOCKETCLIENT_H
