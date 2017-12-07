#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sslserver.h"
#include "blefinder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    SslServer *m_server;
    BleFinder *m_bleFinder;

private slots:
    void on_actionQuit_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
