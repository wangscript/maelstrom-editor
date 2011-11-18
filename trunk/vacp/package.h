#ifndef PACKAGE_H
#define PACKAGE_H

#include <QList>

class QString;
class ContentInfo;

class Package
{
private:
    int id;
    QString &name;
    QList<ContentInfo*> *content;

    Package(int id, QString &name);
public:
    static Package *get_by_name(QString &name);

    QList<ContentInfo*> *get_contentinfo();
    int get_id();
    QString &get_name();
    quint32 get_name_hash();
};

#endif // PACKAGE_H
