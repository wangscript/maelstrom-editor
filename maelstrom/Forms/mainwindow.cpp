#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <resourceeditor.h>
#include <../maelstromproject.h>
#include <assetdbwizard.h>
#include <newprojectwizard.h>

#include <QTextEdit>
#include <QThread>
#include <QX11Info>
#include <QFileInfo>
#include <QWizard>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QFileDialog>

#define ASSERTIONS_ENABLED 1


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->root = NULL;
    this->resourceEditor = NULL;
    this->project = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete this->root;
    if(this->resourceEditor != NULL)
        delete this->resourceEditor;
    if(this->project != NULL)
        delete this->project;
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
    connect(this->ui->actionResource_editor, SIGNAL(triggered()), this, SLOT(openResourceEditor()));
    connect(this->ui->actionNew_project, SIGNAL(triggered()), this, SLOT(newProjectClicked()));
    connect(this->ui->actionOpen, SIGNAL(triggered()), this, SLOT(openProjectClicked()));

    emit this->initializeProgressChanged(tr("Initializing engine..."));
    this->root = new core::Root();

    core::WindowCreationParams wndParams;
    graphics::RenderCreationParams rndParams;
    wndParams.windowHandle = this->viewport[0]->winId();
#if defined(VTX_PLATFORM_LINUX)
    wndParams.displayX11 = QX11Info::display();
#endif
    wndParams.windowTitle = L"Vortex window";
    wndParams.windowSize.x = 800;
    wndParams.windowSize.y = 600;
    rndParams.backBufferSize.x = 800;//this->width();
    rndParams.backBufferSize.y = 600;//this->height();

    rndParams.multisampleCount = 1;
    rndParams.multisampleQuality = 0;
#if defined(VTX_PLATFORM_WIN32)
    rndParams.rapi = graphics::E_RAPI_DX10;
#elif defined(VTX_PLATFORM_LINUX)
    rndParams.rapi = graphics::E_RAPI_OPENGL;
#endif
    this->root->toolInit(wndParams, rndParams);

    emit this->initializeProgressChanged(tr("Setting up project..."));
}

void MainWindow::viewportRedraw()
{
    if(this->root != NULL)
    {
       this->root->toolStep(1.0f);
    }
}

void MainWindow::openResourceEditor()
{
    if(this->project->getAssetDbPath().isEmpty())
    {
        AssetDbWizard wizard;
        if(wizard.exec())
        {
            this->project->setAssetDbPath(wizard.getDbPath());
        }

    }
    if(this->resourceEditor == NULL)
        this->resourceEditor = new ResourceEditor(this->project, this);
    this->resourceEditor->show();
}

void MainWindow::projectInfoChanged()
{
    QString fileName;
    if(this->project->getProjectPath().isEmpty())
    {
        fileName = QString("Unsaved");
    }
    else
    {
        QFileInfo fileInfo(this->project->getProjectPath());
        fileName = fileInfo.fileName();
    }

    this->setWindowTitle(QCoreApplication::applicationName() + " - " + this->project->getName() + " (" + fileName + ")" );
}

bool MainWindow::saveProject()
{
    bool userCanceled = false;
    if(this->project != NULL && !this->project->getInSync())
    {
        QMessageBox box;
        box.setText(tr("Save changes made to ") + this->project->getName() + "?");
        box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Save);
        box.setIcon(QMessageBox::Question);
        int result = box.exec();
        if(result == QMessageBox::Save)
        {
            this->project->save();
        }
        else if(result == QMessageBox::Cancel)
        {
            userCanceled = true;
        }
    }

    return userCanceled;
}

void MainWindow::newProjectClicked(void)
{
    bool userCanceled = this->saveProject();

    if(!userCanceled)
    {
        NewProjectWizard wizard;
        if(wizard.exec())
        {
            AssetDbWizard assetWizard;
            if(assetWizard.exec())
            {
                if(this->project != NULL && !userCanceled)
                {
                    delete this->project;
                }

                QString name = wizard.field("projectname").toString();
                QString projectLocation = wizard.field("projectlocation").toString();
                QString assetDbLocation = assetWizard.field("dblocation").toString();
                bool createDirectory = wizard.field("createdirectory").toBool();

                this->project = MaelstromProject::create(projectLocation, name, createDirectory, assetDbLocation);
                this->project->resetAssetDatabase();

                connect(this->project, SIGNAL(nameChanged()), this, SLOT(projectInfoChanged()));
                connect(this->project, SIGNAL(projectPathChanged()), this, SLOT(projectInfoChanged()));

                projectInfoChanged();
            }
        }
    }
}

void MainWindow::openProjectClicked(void)
{
    bool userCanceled = this->saveProject();

    if(this->project != NULL && !userCanceled)
    {
        delete this->project;
    }

    if(!userCanceled)
    {
        QFileDialog dialog(this,tr("Open Maelstrom Project..."), QString(), "Maelstrom Project (*.mae);;All files (*.*)");
        dialog.setFileMode(QFileDialog::AnyFile);
        if(dialog.exec())
        {
            this->project = MaelstromProject::open(dialog.selectedFiles().at(0));

            connect(this->project, SIGNAL(nameChanged()), this, SLOT(projectInfoChanged()));
            connect(this->project, SIGNAL(projectPathChanged()), this, SLOT(projectInfoChanged()));
            projectInfoChanged();
        }
    }
}
