#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include<QSplashScreen>

namespace Ui {
    class SplashScreen;
}

class SplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = 0);
    ~SplashScreen();
public slots:
    void showMessage(const QString &message);

private:
    Ui::SplashScreen *ui;
};

#endif // SPLASHSCREEN_H
