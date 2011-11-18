#ifndef ASSETDBWIZARD_H
#define ASSETDBWIZARD_H

#include <QWizard>

class AssetDbWizard : public QWizard
{
public:

    enum
    {
        PAGE_INFO,
        PAGE_NEWEXISTING,
        PAGE_NEW,
        PAGE_EXISTING,
        PAGE_NEW_SQLITE
    };
    AssetDbWizard();
    QString getDbPath() const;
};

#endif // ASSETDBWIZARD_H
