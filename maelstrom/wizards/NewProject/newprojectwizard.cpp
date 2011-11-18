#include "newprojectwizard.h"
#include <newprojectwizard.h>
#include <newprojectfirst.h>
#include <QCoreApplication>

NewProjectWizard::NewProjectWizard()
{
    this->setWindowTitle(QCoreApplication::applicationName() + " - New project...");
    this->setPage(PAGE_FIRST, new NewProjectFirst());
}
