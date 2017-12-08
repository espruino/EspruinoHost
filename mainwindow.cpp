#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->logView->append("");
    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addLogMessage(QString html) {
    ui->logView->append(html);
}

void MainWindow::setStatus(QString msg) {
    ui->statusBar->showMessage(msg);
}

void MainWindow::on_actionQuit_triggered()
{
    exit(0);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Espruino Local Host",
                        "Espruino Local Host is a WebSocket\n"
                        "Server that allows http://espruino.com/ide\n"
                       "to access local Serial and Bluetooth Low\n"
                       "Energy devices on your PC.\n"
                       "\n"
                       "It is built using Qt (qt.io) and all source files\n"
                       "are available at:\n"
                       "http://github.com/espruino/EspruinoHost");
}
