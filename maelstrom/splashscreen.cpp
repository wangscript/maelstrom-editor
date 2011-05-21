#include "splashscreen.h"
#include "ui_splashscreen.h"
#include <QPixMap>
#include <QStringBuilder>
#include <platform/vtx_defineconfig.h>


SplashScreen::SplashScreen(QWidget *parent) :
    QSplashScreen(),
    ui(new Ui::SplashScreen)
{
    ui->setupUi(this);
    this->setPixmap(QPixmap(QString(":/images/maelstrom-splash.png")));
}

SplashScreen::~SplashScreen()
{
    delete ui;
}

void SplashScreen::showMessage(const QString &message)
{
    QString fullMessage = QString("Copyright 2011 Emil Nordén\n") % QString("Vortex engine v") % QString(VTX_VER) % QString(" (32 bit)\n") % message;
    QSplashScreen::showMessage(fullMessage, Qt::AlignBottom, Qt::white);
    QApplication::processEvents();
}
