#ifndef TEXTINPUTDIALOG_H
#define TEXTINPUTDIALOG_H

#include <QDialog>

namespace Ui {
    class TextInputDialog;
}

class TextInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextInputDialog(const QString title, const QString message, bool allowEmpty, QWidget *parent = 0);
    ~TextInputDialog();

    QString getText() const;
private:
    Ui::TextInputDialog *ui;
private slots:
    void textChanged();
};

#endif // TEXTINPUTDIALOG_H
