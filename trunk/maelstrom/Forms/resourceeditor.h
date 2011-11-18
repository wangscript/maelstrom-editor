#ifndef RESOURCEEDITOR_H
#define RESOURCEEDITOR_H

#include <QDialog>

class MaelstromProject;
class QSqlQueryModel;
class PackageListWidget;
class QBoxLayout;
class QTableWidget;


namespace Ui {
    class resourceeditor;
}

class ResourceEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ResourceEditor(MaelstromProject *project, QWidget *parent = 0);
    ~ResourceEditor();

private:
    Ui::resourceeditor *ui;
    MaelstromProject *project;
    PackageListWidget *packageList;
    QBoxLayout *mainLayout;
    QBoxLayout *verticalLayout;
    QTableWidget *settingWidget;
};

#endif // RESOURCEEDITOR_H
