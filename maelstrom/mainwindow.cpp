#define VTX_TOOL_BUILD
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextEdit>
#include <QThread>

#define ASSERTIONS_ENABLED 1


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->root = NULL;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    emit this->initializeProgressChanged(tr("Setting up workspace..."));

    this->topSplitter = new QSplitter(Qt::Horizontal);
    this->bottomSplitter = new QSplitterEx(Qt::Horizontal);
    this->viewport[0] = new Viewport(this->topSplitter);
    /*this->viewport[1] = new Viewport(this->topSplitter);
    this->viewport[2] = new Viewport(this->bottomSplitter);
    this->viewport[3] = new Viewport(this->bottomSplitter);*/
    this->topSplitter->addWidget(this->viewport[0]);
    //this->topSplitter->addWidget(this->viewport[1]);


    //this->bottomSplitter->addWidget(this->viewport[2]);
    //this->bottomSplitter->addWidget(this->viewport[3]);

    this->verticalSplitter = new QSplitter(Qt::Vertical);
    this->verticalSplitter->addWidget(this->topSplitter);
    this->verticalSplitter->addWidget(this->bottomSplitter);
    this->ui->gridLayout->addWidget(this->verticalSplitter);

    connect(topSplitter, SIGNAL(splitterMoved(int,int)), this->bottomSplitter, SLOT(moveMySplitter(int,int)));
    connect(this->viewport[0], SIGNAL(redraw()), this, SLOT(viewportRedraw()));

    emit this->initializeProgressChanged(tr("Initializing engine..."));
    this->root = new Root();

    WindowCreationParams wndParams;
    RenderCreationParams rndParams;
    wndParams.windowHandle = this->viewport[0]->winId();
    rndParams.backBufferSize.x = this->width();
    rndParams.backBufferSize.y = this->height();
    rndParams.multisampleCount = 1;
    rndParams.multisampleQuality = 0;
    rndParams.rapi = E_RAPI_DX10;

    this->root->toolInit(wndParams, rndParams);
}

void MainWindow::viewportRedraw()
{
    if(this->root != NULL)
    {
        this->root->toolStep(1.0f);
    }
}
