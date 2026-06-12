#include <iostream>
#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    std::cout << "main() start\n";

    // ⭐ Force REAL OpenGL (not ANGLE)
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    // ⭐ Global format for ALL windows (OCCT requires CompatibilityProfile)
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    fmt.setVersion(3, 3);
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);

    std::cout << "Constructing MainWindow\n";
    MainWindow w;
    std::cout << "Showing MainWindow\n";
    w.show();

    std::cout << "Entering event loop\n";
    int rc = app.exec();
    std::cout << "Event loop exited with code " << rc << "\n";

    return rc;
}
