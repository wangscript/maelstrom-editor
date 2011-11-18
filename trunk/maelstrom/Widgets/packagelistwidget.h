#ifndef PACKAGELISTWIDGET_H
#define PACKAGELISTWIDGET_H

#include <QWidget>

class MaelstromProject;
class QSqlQueryModel;

namespace Ui {
    class PackageListWidget;
}

class PackageListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PackageListWidget(MaelstromProject *project, QWidget *parent = 0);
    ~PackageListWidget();

private:
    Ui::PackageListWidget *ui;
    MaelstromProject *project;
    QSqlQueryModel *packageModel;

    void updateList();

private slots:
    void addPackageClicked();
    void removePackageClicked();
};

#endif // PACKAGELISTWIDGET_H
