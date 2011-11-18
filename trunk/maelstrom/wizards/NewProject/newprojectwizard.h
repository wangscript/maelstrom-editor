#ifndef NEWPROJECTWIZARD_H
#define NEWPROJECTWIZARD_H
#include <QWizard>

class NewProjectWizard : public QWizard
{
public:
    enum
    {
        PAGE_FIRST
    };
    NewProjectWizard();
};

#endif // NEWPROJECTWIZARD_H
