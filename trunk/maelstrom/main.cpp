#include <QtGui/QApplication>
#include <splashscreen.h>
#include <mainwindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Vortex");
    QCoreApplication::setApplicationName("Maelstrom");

    SplashScreen splash;
    MainWindow mainWindow;

    QObject::connect(&mainWindow, SIGNAL(initializeProgressChanged(QString)), &splash, SLOT(showMessage(QString)));

    splash.show();
    mainWindow.initialize();

    mainWindow.show();

    splash.close();

    return a.exec();
}
