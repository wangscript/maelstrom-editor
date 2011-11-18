#include "assetdbnewexisting.h"
#include <QRadioButton>
#include <QBoxLayout>
#include <assetdbwizard.h>

AssetDbNewExisting::AssetDbNewExisting()
{
    this->setTitle(tr("New or existing asset database"));
    this->setSubTitle(tr("Do you want to use an existing asset database or create a new one?"));

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    this->setLayout(layout);

    this->existingButton    = new QRadioButton(tr("Existing database"), this);
    this->newButton         = new QRadioButton(tr("New database"), this);
    existingButton->setChecked(true);

    layout->addWidget(this->existingButton);
    layout->addWidget(this->newButton);
}

int AssetDbNewExisting::nextId() const
{
    if(this->existingButton->isChecked())
        return AssetDbWizard::PAGE_EXISTING;
    else
        return AssetDbWizard::PAGE_NEW_SQLITE;
}
