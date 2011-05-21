#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <viewport.h>
#include <QSplitter>
#include <QTimer>

#include <vortex/vtx_vortex.h>
#include <vortex/vtx_window.h>

namespace Ui {
    class MainWindow;
}

class QSplitterEx : public QSplitter
{
    Q_OBJECT
public:
    QSplitterEx(Qt::Orientation orientation) : QSplitter(orientation, 0)
    {}
public slots:
    void moveMySplitter(int i, int j)
    {
        this->moveSplitter(i, j);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initialize();

public slots:
    void viewportRedraw(void);
signals:
    void initializeProgressChanged(QString);

private:
    Ui::MainWindow *ui;
    QSplitterEx *bottomSplitter;
    QSplitter *topSplitter, *verticalSplitter;
    Viewport *viewport[4];
    Root *root;
};

#endif // MAINWINDOW_H
