#ifndef APP_H
#define APP_H

#include <QObject>

QT_FORWARD_DECLARE_CLASS(SslServer)
QT_FORWARD_DECLARE_CLASS(BleFinder)
QT_FORWARD_DECLARE_CLASS(MainWindow)

#define APP_VERSION "0.1"
#define WEBSOCKET_PORT 31234
#define BLE_MAX_WRITE_SIZE 20

class App : public QObject
{
    Q_OBJECT
public:
    explicit App(QObject *parent = nullptr);
    void init();
    ~App();

    SslServer *m_server;
    BleFinder *m_bleFinder;

    MainWindow *m_mainWindow;
    void gui(); // start GUI mode

    void status(QString msg);
    void log(QString msg);
    void warn(QString msg);
    void error(QString msg);

signals:

public slots:
};

extern App *g_app;

#endif // APP_H
