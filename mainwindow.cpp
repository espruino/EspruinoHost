#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bleFinder = new BleFinder(this);
    m_server = new SslServer(this, this);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_bleFinder;
    delete m_server;
}

void MainWindow::on_actionQuit_triggered()
{
    exit(0);
}
