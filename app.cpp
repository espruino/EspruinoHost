#include "app.h"
#include "blefinder.h"
#include "sslserver.h"
#include "mainwindow.h"

#include <QtCore/QDebug>

App::App(QObject *parent) : QObject(parent)
{
    m_mainWindow = 0;
    m_bleFinder = 0;
    m_server = 0;
}

void App::init() {
    status("Espruino Host v" APP_VERSION);
    m_bleFinder = new BleFinder(this);
    m_server = new SslServer(this);
}

App::~App() {
    delete m_mainWindow;
    m_mainWindow = 0;
    delete m_bleFinder;
    delete m_server;
}

void App::gui() {
    m_mainWindow = new MainWindow();
    m_mainWindow->show();
}

void App::status(QString msg) {
    qDebug() << "STATUS:" << msg;
    if (m_mainWindow) {
        m_mainWindow->addLogMessage("<p style=\"padding:0;margin:0;\"><b>"+msg+"</b></p>\n");
        m_mainWindow->setStatus(msg);
    }
}

void App::log(QString msg) {
    qDebug() << msg;
    if (m_mainWindow)
        m_mainWindow->addLogMessage("<p style=\"padding:0;margin:0;\">"+msg+"</p>\n");
}
void App::warn(QString msg) {
    qDebug() << "WARN:" << msg;
    if (m_mainWindow)
        m_mainWindow->addLogMessage("<p style=\"padding:0; margin:0;color:yellow\">"+msg+"</p>\n");
}
void App::error(QString msg) {
    qDebug() << "ERROR:" << msg;
    if (m_mainWindow)
        m_mainWindow->addLogMessage("<p style=\"padding:0;margin:0;color:red;\">"+msg+"</p>\n");
}
