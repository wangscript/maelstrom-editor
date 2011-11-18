#include "textinputdialog.h"
#include "ui_textinputdialog.h"
#include <QPushButton>

TextInputDialog::TextInputDialog(const QString title, const QString message, bool allowEmpty, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextInputDialog)
{
    ui->setupUi(this);

    this->setWindowTitle(title);
    this->ui->labelMessage->setText(message);

    if(!allowEmpty)
        connect(this->ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
}

TextInputDialog::~TextInputDialog()
{
    delete ui;
}

void TextInputDialog::textChanged()
{
    this->ui->buttonBox->buttons().at(0)->setEnabled(!this->ui->lineEdit->text().isEmpty());
}

QString TextInputDialog::getText() const
{
    return this->ui->lineEdit->text();
}
