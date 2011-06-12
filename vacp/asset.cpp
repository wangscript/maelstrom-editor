#include <asset.h>
#include <error.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>

Asset::Asset(int asset_id, QString &asset_name, QString &asset_source, QDateTime &asset_last_built, QString &asset_output, QString &asset_exporter, QString &asset_compiler)
    :	id(asset_id),
      name(asset_name),
      source(asset_source),
      last_built(asset_last_built),
      output(asset_output),
      exporter(asset_exporter),
      compiler(asset_compiler)
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
    QString sql("SELECT id, name, source, modified, output FROM assets WHERE name='");
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
    QString &asset_name = query.value(1).toString();
    QString &source = query.value(2).toString();
    QDateTime &last_built = query.value(3).toDateTime();
    QString &output = query.value(4).toString();
    QString &exporter = query.value(5).toString();
    QString &compiler = query.value(6).toString();

    return new Asset(
                id,
                asset_name,
                source,
                last_built,
                output,
                exporter,
                compiler);
}

std::vector<Asset*> *Asset::get_dependencies(void)
{
    QSqlQuery query;
    QString sql("SELECT id, name, source, modified, output FROM assets INNER JOIN asset_dependencies ad ON assets.id = ad.child_id WHERE ad.parent_id = ");
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
        QString &asset_name = query.value(1).toString();
        QString &source = query.value(2).toString();
        QDateTime &last_built = query.value(3).toDateTime();
        QString &output = query.value(4).toString();
        QString &exporter = query.value(5).toString();
        QString &compiler = query.value(6).toString();

        deps->push_back(new Asset(id, asset_name, source, last_built, output, exporter, compiler));
    }

    return deps;
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
