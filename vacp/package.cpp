#include <package.h>
#include <content.h>
#include <error.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QList>
#include <utilities.h>

#define PACKAGE_COLUMNS   "id, name"

Package::Package(int package_id, QString &package_name)
    : id(package_id),
      name(package_name)
{

}

QList<ContentInfo*> *Package::get_contentinfo()
{
    return this->content;
}

Package *Package::get_by_name(QString &name)
{
    QSqlQuery query;
    QString sql("SELECT ");
    sql.append(PACKAGE_COLUMNS);
    sql.append(" FROM packages WHERE name='");
    sql.append(name);
    sql.append("'");

    if(!query.exec(sql))
    {
        QSqlError error = query.lastError();
        QString message(error.text());
        throw new DBException(message);
    }

    if(!query.next())
    {
        throw new NoSuchPackageException(name);
    }

    int id = query.value(0).toInt();
    QString *package_name = new QString(query.value(1).toString());

    Package *package = new Package(id, *package_name);
    package->content = ContentInfo::get_by_package_id(id);

    return package;
}


int Package::get_id()
{
    return this->id;
}

QString &Package::get_name()
{
    return this->name;
}

quint32 Package::get_name_hash()
{
    return Utilities::calc_str_hash(this->name.toLocal8Bit().data());
}
