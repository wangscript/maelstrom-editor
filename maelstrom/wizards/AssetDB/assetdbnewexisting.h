#ifndef ASSETDBNEWEXISTING_H
#define ASSETDBNEWEXISTING_H

#include <QWizardPage>

class QRadioButton;

class AssetDbNewExisting : public QWizardPage
{
private:
    QRadioButton *existingButton;
    QRadioButton *newButton;
public:
    AssetDbNewExisting();
protected:
    virtual int nextId() const;
};

#endif // ASSETDBNEWEXISTING_H
