#include <content.h>
#include <error.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QDateTime>
#include <utilities.h>

#define CONTENT_COLUMNS   "id, name, source, last_built, output, exporter, compiler, force_rebuild, compileconfig, package_id"

ContentInfo::ContentInfo(int content_id, QString &content_name, QString &content_source, QDateTime &content_last_built, QString &content_output, QString &content_exporter, QString &content_compiler, bool content_force_build, QString &content_compileconfig, int package_id)
    :	id(content_id),
      name(content_name),
      source(content_source),
      last_built(content_last_built),
      output(content_output),
      exporter(content_exporter),
      compiler(content_compiler),
      force_build(content_force_build),
      compileconfig(content_compileconfig),
      package_id(package_id)
{
    this->dependencies = NULL;
    this->package_name = NULL;
    this->validate();
}

ContentInfo::~ContentInfo()
{
    delete &this->name;
    delete &this->last_built;
    delete &this->output;
    delete &this->source;
    delete &this->exporter;
    delete &this->compiler;
    delete &this->compileconfig;

    // Delete the dependency list, but not the depencies themselves.
    delete this->dependencies;
}

ContentInfo* ContentInfo::get_by_name(QString &name)
{
    QSqlQuery query;
    QString sql("SELECT ");
    sql.append(CONTENT_COLUMNS);
    sql.append(" FROM content WHERE name='");
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
        throw new NoSuchContentException(name);
    }

    int id = query.value(0).toInt();
    QString *content_name = new QString(query.value(1).toString());
    QString *source = new QString(query.value(2).toString());
    QDateTime *last_built = new QDateTime(query.value(3).toDateTime());
    QString *output = new QString(query.value(4).toString());
    QString *exporter = new QString(query.value(5).toString());
    QString *compiler = new QString(query.value(6).toString());
    bool force_build = query.value(7).toBool();
    QString *compileconfig = new QString(query.value(8).toString());
    int package_id = query.value(9).toInt();

    return new ContentInfo(
                id,
                *content_name,
                *source,
                *last_built,
                *output,
                *exporter,
                *compiler,
                force_build,
                *compileconfig,
                package_id);
}

QList<ContentInfo*> *ContentInfo::get_by_package_id(int id)
{
    QSqlQuery query;
    QString sql("SELECT ");
    sql.append(CONTENT_COLUMNS);
    sql.append(" FROM content WHERE content.package_id = " + QString::number(id));

    if(!query.exec(sql))
    {
        QSqlError error = query.lastError();
        QString message(error.text());
        throw new DBException(message);
    }

    QList<ContentInfo*> *contents = new QList<ContentInfo*>;
    while(query.next())
    {
        int id = query.value(0).toInt();
        QString *content_name = new QString(query.value(1).toString());
        QString *source = new QString(query.value(2).toString());
        QDateTime *last_built = new QDateTime(query.value(3).toDateTime());
        QString *output = new QString(query.value(4).toString());
        QString *exporter = new QString(query.value(5).toString());
        QString *compiler = new QString(query.value(6).toString());
        bool force_build = query.value(7).toBool();
        QString *compileconfig = new QString(query.value(8).toString());
        int package_id = query.value(9).toInt();

        contents->push_back(new ContentInfo(
                              id,
                              *content_name,
                              *source,
                              *last_built,
                              *output,
                              *exporter,
                              *compiler,
                              force_build,
                              *compileconfig,
                              package_id));
    }

    return contents;
}

QString &ContentInfo::get_compileconfig()
{
    return this->compileconfig;
}

QString &ContentInfo::get_compiler()
{
    return this->compiler;
}

QList<ContentInfo*> *ContentInfo::get_dependencies(void)
{
    if(this->dependencies == NULL)
        this->load_dependencies();
    return this->dependencies;
}

QString &ContentInfo::get_exporter()
{
    return this->exporter;
}

bool ContentInfo::get_force_build()
{
    return this->force_build;
}

quint32 ContentInfo::get_id() const
{
    return this->id;
}

QDateTime &ContentInfo::get_last_built()
{
    return this->last_built;
}

QString &ContentInfo::get_name() const
{
    return this->name;
}

quint32 ContentInfo::get_name_hash() const
{
    return Utilities::calc_str_hash(this->get_name().toLocal8Bit().data());
}

QString &ContentInfo::get_output()
{
    return this->output;
}

QString *ContentInfo::get_package_name()
{
    if(this->package_name == NULL)
        this->load_package_name();
    return this->package_name;
}

QString &ContentInfo::get_source()
{
    return this->source;
}

void ContentInfo::load_dependencies()
{
    QSqlQuery query;
    QString sql("SELECT ");
    sql.append(CONTENT_COLUMNS);
    sql.append(" FROM content INNER JOIN content_dependencies ad ON content.id = ad.child_id WHERE ad.parent_id = ");
    sql.append(QString::number(this->id));

    if(!query.exec(sql))
    {
        QSqlError error = query.lastError();
        QString message(error.text());
        throw new DBException(message);
    }

    QList<ContentInfo*> *deps = new QList<ContentInfo*>;
    while(query.next())
    {
        int id = query.value(0).toInt();
        QString *content_name = new QString(query.value(1).toString());
        QString *source = new QString(query.value(2).toString());
        QDateTime *last_built = new QDateTime(query.value(3).toDateTime());
        QString *output = new QString(query.value(4).toString());
        QString *exporter = new QString(query.value(5).toString());
        QString *compiler = new QString(query.value(6).toString());
        bool force_build = query.value(7).toBool();
        QString *compileconfig = new QString(query.value(8).toString());

        deps->push_back(new ContentInfo(id, *content_name, *source, *last_built, *output, *exporter, *compiler, force_build, *compileconfig, package_id));
    }

    this->dependencies = deps;
}

void ContentInfo::load_package_name()
{
    QString *package_name = NULL;
    QSqlQuery query;
    QString sql("SELECT name FROM packages where id = " + QString::number(this->package_id));

    if(!query.exec(sql))
    {
        QSqlError error = query.lastError();
        QString message(error.text());
        throw new DBException(message);
    }

    if(!query.next())
    {
        throw new InvalidContentException(this->name, "Content refers to a packet that does not exist");
    }

    package_name = new QString(query.value(0).toString());

    this->package_name = package_name;
}

void ContentInfo::validate(void)
{
    if(this->name.isEmpty())
    {
        throw new InvalidContentException(QString("(Database ID) ") + QString::number(this->id), "Content name can not be empty");
    }
    if(this->source.isEmpty())
    {
        throw new InvalidContentException(this->name, "Content source path can not be empty");
    }
    if(!QFile::exists(this->source))
    {
        throw new InvalidContentException(this->name, "Content source does not exist");
    }
}
