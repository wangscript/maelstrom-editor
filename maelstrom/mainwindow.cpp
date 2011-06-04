#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QTextEdit>
#include <QThread>
#include <QX11Info>


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
    WId id;
    wndParams.windowHandle = this->viewport[0]->winId();
#if defined(VTX_PLATFORM_LINUX)
    wndParams.displayX11 = QX11Info::display();
#endif
    wndParams.windowTitle = L"Vortex window";
    wndParams.windowSize.x = 800;
    wndParams.windowSize.y = 600;
    rndParams.backBufferSize.x = 1;//this->width();
    rndParams.backBufferSize.y = 1;//this->height();

    rndParams.multisampleCount = 1;
    rndParams.multisampleQuality = 0;
#if defined(VTX_PLATFORM_WIN32)
    rndParams.rapi = E_RAPI_DX10;
#elif defined(VTX_PLATFORM_LINUX)
    rndParams.rapi = E_RAPI_OPENGL;
#endif

    //this->root->Run(wndParams, rndParams);
    this->root->toolInit(wndParams, rndParams);
}

void MainWindow::viewportRedraw()
{
    if(this->root != NULL)
    {
       this->root->toolStep(1.0f);
    }
}
