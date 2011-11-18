#include "packagelistwidget.h"
#include "ui_packagelistwidget.h"
#include <QSqlQueryModel>
#include <textinputdialog.h>
#include <../maelstromproject.h>

PackageListWidget::PackageListWidget(MaelstromProject *project, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PackageListWidget)
{
    ui->setupUi(this);
    this->project = project;
    this->packageModel = new QSqlQueryModel();

    this->updateList();

    connect(this->ui->addButton, SIGNAL(clicked()), this, SLOT(addPackageClicked()));
}

PackageListWidget::~PackageListWidget()
{
    delete ui;

    delete this->packageModel;
}

void PackageListWidget::updateList()
{
    this->packageModel->setQuery("SELECT * FROM packages");
    this->ui->packageList->setModel(this->packageModel);
    this->ui->packageList->setModelColumn(1);
}

void PackageListWidget::addPackageClicked()
{
    TextInputDialog dialog(tr("New package"), tr("Package name:"), false, this);
    if(dialog.exec())
    {
        this->project->addAssetPackage(dialog.getText());
        this->updateList();
    }
}

void PackageListWidget::removePackageClicked()
{

}
