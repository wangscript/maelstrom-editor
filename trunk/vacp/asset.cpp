#include <asset.h>
#include <error.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QTextCodec>

#define ASSET_COLUMNS   "id, name, source, modified, output, exporter, compiler, force_rebuild, compileconfig"

Asset::Asset(int asset_id, QString &asset_name, QString &asset_source, QDateTime &asset_last_built, QString &asset_output, QString &asset_exporter, QString &asset_compiler, bool asset_force_build, QString &asset_compileconfig)
    :	id(asset_id),
      name(asset_name),
      source(asset_source),
      last_built(asset_last_built),
      output(asset_output),
      exporter(asset_exporter),
      compiler(asset_compiler),
      force_build(asset_force_build),
      compileconfig(asset_compileconfig)
{
    this->validate();
}

Asset::~Asset()
{
    delete &this->name;
    delete &this->last_built;
    delete &this->output;
    delete &this->source;
    delete &this->exporter;
    delete &this->compiler;
}

void Asset::validate(void)
{
    char *ss = this->source.toLocal8Bit().data();
    if(this->name.isEmpty())
    {
        QString message("Asset name can not be empty\n");
        throw new InvalidAssetException(message);
    }
    if(this->source.isEmpty())
    {
        QString message("Asset source path can not be empty");
        throw new InvalidAssetException(message);
    }
    if(!QFile::exists(this->source))
    {
        QString message("Asset source does not exist");
        throw new InvalidAssetException(message);
    }
}

Asset* Asset::get_by_name(QString &name)
{
    QSqlQuery query;
    QString sql("SELECT ");
    sql.append(ASSET_COLUMNS);
    sql.append(" FROM assets WHERE name='");
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
        throw new NoSuchAssetException;
    }

    int id = query.value(0).toInt();
    QString *asset_name = new QString(query.value(1).toString());
    QString *source = new QString(query.value(2).toString());
    QDateTime *last_built = new QDateTime(query.value(3).toDateTime());
    QString *output = new QString(query.value(4).toString());
    QString *exporter = new QString(query.value(5).toString());
    QString *compiler = new QString(query.value(6).toString());
    bool force_build = query.value(7).toBool();
    QString *compileconfig = new QString(query.value(8).toString());


    /*const char* source_a = (source->codecForCStrings()
    char first = source_a[0];
    char second = source_a[1];

    qint32 sho = *((qint32*)source_a);*/

    return new Asset(
                id,
                *asset_name,
                *source,
                *last_built,
                *output,
                *exporter,
                *compiler,
                force_build,
                *compileconfig);
}

std::vector<Asset*> *Asset::get_dependencies(void)
{
    QSqlQuery query;
    QString sql("SELECT ");
    sql.append(ASSET_COLUMNS);
    sql.append(" FROM assets INNER JOIN asset_dependencies ad ON assets.id = ad.child_id WHERE ad.parent_id = ");
    sql.append(QString::number(this->id));

    if(!query.exec(sql))
    {
        QSqlError error = query.lastError();
        QString message(error.text());
        throw new DBException(message);
    }

    std::vector<Asset*> *deps = new std::vector<Asset*>;
    while(query.next())
    {
        int id = query.value(0).toInt();
        QString *asset_name = new QString(query.value(1).toString());
        QString *source = new QString(query.value(2).toString());
        QDateTime *last_built = new QDateTime(query.value(3).toDateTime());
        QString *output = new QString(query.value(4).toString());
        QString *exporter = new QString(query.value(5).toString());
        QString *compiler = new QString(query.value(6).toString());
        bool force_build = query.value(7).toBool();
        QString *compileconfig = new QString(query.value(8).toString());

        deps->push_back(new Asset(id, *asset_name, *source, *last_built, *output, *exporter, *compiler, force_build, *compileconfig));
    }

    return deps;
}

QString &Asset::get_name()
{
    return this->name;
}

QString &Asset::get_output()
{
    return this->output;
}

QString &Asset::get_source()
{
    return this->source;
}

QString &Asset::get_exporter()
{
    return this->exporter;
}

QString &Asset::get_compiler()
{
    return this->compiler;
}

QDateTime &Asset::get_last_built()
{
    return this->last_built;
}

bool &Asset::get_force_build()
{
    return this->force_build;
}

QString &Asset::get_compileconfig()
{
    return this->compileconfig;
}
