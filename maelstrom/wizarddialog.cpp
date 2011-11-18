#include "wizarddialog.h"
#include "ui_wizarddialog.h"

WizardDialog::WizardDialog( WizardPageBase *wizardPage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WizardDialog)
{
    ui->setupUi(this);

    this->ui->verticalLayout->addWidget(reinterpret_cast<QWidget*>(wizardPage));
}

WizardDialog::~WizardDialog()
{
    delete ui;
}
