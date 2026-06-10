#include <iostream>
#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    std::cout << "main() start\n";

    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setVersion(3, 3);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);

    std::cout << "Constructing MainWindow\n";
    MainWindow w;
    std::cout << "Showing MainWindow\n";
    w.show();
    w.move(0, 0);
    w.raise();
    w.activateWindow();


    std::cout << "Entering event loop\n";
    int rc = app.exec();
    std::cout << "Event loop exited with code " << rc << "\n";

    return rc;
}