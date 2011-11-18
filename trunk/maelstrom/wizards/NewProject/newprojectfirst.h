#ifndef NEWPROJECTFIRST_H
#define NEWPROJECTFIRST_H
#include <QWizardPage>

class QLineEdit;
class QCheckBox;
class QFormLayout;
class QBoxLayout;
class QPushButton;

class NewProjectFirst : public QWizardPage
{
    Q_OBJECT
private:
    QLineEdit *textName;
    QLineEdit *textLocation;
    QCheckBox *checkCreateDir;
    QFormLayout *formLayout;
    QBoxLayout *locationLayout;
    QPushButton *browseButton;
public:
    NewProjectFirst();
public slots:
    void browseButtonClicked();
};

#endif // NEWPROJECTFIRST_H
