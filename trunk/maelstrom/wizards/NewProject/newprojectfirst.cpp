#include "newprojectfirst.h"
#include <QBoxLayout>
#include <QFormLayout>
#include <QSpacerItem>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>

NewProjectFirst::NewProjectFirst()
{
    this->setTitle("New project");
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    this->setLayout(layout);
    //QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //layout->addItem(spacer);
    //

    this->formLayout = new QFormLayout(this);

    this->textName = new QLineEdit(this);
    this->formLayout->addRow(tr("Project name"), this->textName);

    this->textLocation = new QLineEdit(this);
    this->browseButton = new QPushButton(tr("Browse"), this);
    this->locationLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    this->locationLayout->addWidget(this->textLocation);
    this->locationLayout->addWidget(this->browseButton);
    this->formLayout->addRow(tr("Location"), this->locationLayout);

    this->checkCreateDir = new QCheckBox(tr("Create directory for project"), this);
    this->formLayout->addWidget(this->checkCreateDir);

    layout->addLayout(this->formLayout);

    connect(this->browseButton, SIGNAL(clicked()), this, SLOT(browseButtonClicked()));

    this->registerField("projectname*", this->textName);
    this->registerField("projectlocation*", this->textLocation);
    this->registerField("createdirectory", this->checkCreateDir);
}

void NewProjectFirst::browseButtonClicked()
{
    QFileDialog dialog(this,tr("Select directory for project"));
    dialog.setFileMode(QFileDialog::Directory);
    if(dialog.exec())
    {
        this->textLocation->setText(dialog.selectedFiles().at(0));
    }
}
