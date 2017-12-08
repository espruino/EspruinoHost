#include "app.h"
#include <QApplication>

App *g_app;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    g_app = new App();
    g_app->gui();
    g_app->init();


    return a.exec();
}
