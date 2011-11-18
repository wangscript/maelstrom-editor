#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <viewport.h>
#include <QSplitter>
#include <QTimer>

#include <core/vtx_vortex.h>
#include <core/vtx_windowcreationparams.h>
#include <graphics/vtx_rendercreationparams.h>


namespace Ui {
    class MainWindow;
}

class MaelstromProject;
class ResourceEditor;

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
    void openResourceEditor(void);
    void projectInfoChanged(void);
    void newProjectClicked(void);
    void openProjectClicked(void);
signals:
    void initializeProgressChanged(QString);

private:
    bool saveProject();
    Ui::MainWindow *ui;
    QSplitterEx *bottomSplitter;
    QSplitter *topSplitter, *verticalSplitter;
    Viewport *viewport[4];
    core::Root *root;
    MaelstromProject *project;
    ResourceEditor *resourceEditor;
};

#endif // MAINWINDOW_H
