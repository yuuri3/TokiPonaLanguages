#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}