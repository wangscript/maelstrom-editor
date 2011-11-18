#ifndef CONTENTINFO_H
#define CONTENTINFO_H

#include <QList>
#include <QString>

class QDateTime;


class ContentInfo
{
private:
    int id;
    int package_id;
    QDateTime &last_built;
    QString &name;
    QString &source;
    QString &output;
    QString &exporter;
    QString &compiler;
    bool force_build;
    QString &compileconfig;
    QList<ContentInfo*> *dependencies;
    QString *package_name;

    // Non-persistent members.
    QString build_output;
    QString build_meta_output;

    ContentInfo(int content_id, QString &content_name, QString &content_source, QDateTime &content_last_built, QString &content_output, QString &exporter, QString &compiler, bool force_build, QString &compileconfig, int package_id);
    void load_dependencies();
    void load_package_name();
    void validate();
public:
    ~ContentInfo();
    static ContentInfo* get_by_name(QString &name);
    static QList<ContentInfo*> *get_by_package_id(int id);
    QString &get_compileconfig();
    QString &get_compiler();
    QList<ContentInfo*> *get_dependencies();
    QString &get_exporter();
    bool get_force_build();
    quint32 get_id() const;
    QDateTime &get_last_built();
    QString &get_name() const;
    quint32 get_name_hash() const;
    QString &get_output();
    QString *get_package_name();
    QString &get_source();
};

#endif // CONTENTINFO_H
