#include "assetdbwizard.h"
#include <assetdbinfo.h>
#include <assetdbnewexisting.h>
#include <assetdbnewsqlite.h>
#include <QCoreApplication>
#include <QVariant>


AssetDbWizard::AssetDbWizard()
{
    this->setWindowTitle(QCoreApplication::applicationName() + " Resource Editor");
    this->setPage(PAGE_INFO, new AssetDbInfo());
    this->setPage(PAGE_NEWEXISTING, new AssetDbNewExisting());
    this->setPage(PAGE_NEW_SQLITE, new AssetDbNewSqlite());
}

QString AssetDbWizard::getDbPath() const
{
    return this->field("dblocation*").toString();
}
