#include "assetdbinfo.h"
#include <QBoxLayout>
#include <QSpacerItem>

AssetDbInfo::AssetDbInfo()
{
    this->setTitle("Asset database wizard");
    this->setSubTitle(tr("This wizard will help you set up the asset database for your project.\nClick next to set up a new or existing asset database."));

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    this->setLayout(layout);
    layout->addItem(spacer);
}
