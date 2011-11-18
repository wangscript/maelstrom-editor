#include "resourceeditor.h"
#include "ui_resourceeditor.h"
#include <QSqlDatabase>
#include <QDataWidgetMapper>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QToolBar>
#include <QToolButton>
#include <QBoxLayout>
#include "textinputdialog.h"
#include <packagelistwidget.h>

#include <../maelstromproject.h>

ResourceEditor::ResourceEditor(MaelstromProject *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resourceeditor)
{
    this->project = project;
    ui->setupUi(this);

    this->mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    this->verticalLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    this->packageList = new PackageListWidget(this->project, this);

    this->mainLayout->addWidget(this->packageList);
    this->verticalLayout->addLayout(this->mainLayout);

    this->setLayout(this->verticalLayout);

}

ResourceEditor::~ResourceEditor()
{
    delete ui;

    delete this->packageList;
    delete this->mainLayout;
    delete this->verticalLayout;
}
