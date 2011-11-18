#include "assetdbnewsqlite.h"
#include <QFormLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

AssetDbNewSqlite::AssetDbNewSqlite()
{
    this->setTitle(tr("New asset database"));
    this->setSubTitle(tr("Specify the location and credentials (if any)"));

    QFormLayout *layout             = new QFormLayout(this);
    QFormLayout *credentialLayout   = new QFormLayout;
    QBoxLayout *locationLayout      = new QBoxLayout(QBoxLayout::LeftToRight);

    this->setLayout(layout);

    this->textLocation  = new QLineEdit(this);

    this->buttonBrowse  = new QPushButton(tr("Browse"), this);
    connect(this->buttonBrowse, SIGNAL(clicked()), this, SLOT(browseClicked()));
    this->textUserName  = new QLineEdit(this);
    this->textPassword  = new QLineEdit(this);
    this->textPassword->setEchoMode(QLineEdit::Password);
    this->checkCredentials = new QCheckBox(tr("Use credentials"), this);
    connect(this->checkCredentials, SIGNAL(toggled(bool)), this, SLOT(credentialsChecked(bool)));
    this->checkCredentials->setChecked(true);

    locationLayout->addWidget(this->textLocation);
    locationLayout->addWidget(this->buttonBrowse);

    credentialLayout->addRow(this->checkCredentials);
    credentialLayout->addRow(tr("Username"), this->textUserName);
    credentialLayout->addRow(tr("Password"), this->textPassword);

    layout->addRow(tr("Location"), locationLayout);
    layout->addRow("", credentialLayout);

    this->registerField("dblocation*", this->textLocation);
}


void AssetDbNewSqlite::credentialsChecked(bool checked)
{
    this->textUserName->setEnabled(checked);
    this->textPassword->setEnabled(checked);
}

void AssetDbNewSqlite::browseClicked()
{
    QFileDialog dialog(this,tr("Create new SQLite database..."));
    dialog.setFileMode(QFileDialog::AnyFile);
    if(dialog.exec())
    {
        this->textLocation->setText(dialog.selectedFiles().at(0));
    }

}

bool AssetDbNewSqlite::validatePage()
{
    bool valid = false;
    QFile file(this->textLocation->text());
    if(file.exists())
    {
        QMessageBox box;
        box.setText(tr("The specified file already exists."));
        box.setInformativeText("Its contents will be lost. Continue?");
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::No);
        box.setIcon(QMessageBox::Question);
        if(box.exec() == QMessageBox::Yes)
            valid = true;
    }
    else
    {
        valid = true;
    }

    return valid;
}
