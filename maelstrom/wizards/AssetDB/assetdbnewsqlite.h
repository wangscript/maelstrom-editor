#ifndef ASSETDBNEWSQLITE_H
#define ASSETDBNEWSQLITE_H
#include <QWizardPage>

class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

class AssetDbNewSqlite : public QWizardPage
{
    Q_OBJECT
private:
    QLineEdit *textLocation;
    QLineEdit *textUserName;
    QLineEdit *textPassword;
    QCheckBox *checkCredentials;
    QPushButton *buttonBrowse;

public:
    AssetDbNewSqlite();
public slots:
    void credentialsChecked(bool checked);
    void browseClicked();
    virtual bool validatePage();
};

#endif // ASSETDBNEWSQLITE_H
