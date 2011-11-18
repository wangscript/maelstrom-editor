#include "maelstromproject.h"
#include <QFile>
#include <QDataStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QSqlError>
#include <QVariant>

#define CREATE_PACKAGE_TABLE        "CREATE TABLE packages(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);"
#define CREATE_ASSET_TABLE          "CREATE TABLE content(id INTEGER PRIMARY KEY AUTOINCREMENT, name text not null, source text not null, last_built datetime not null, output text, exporter text, compiler text, force_rebuild bit, compileconfig text, package_id integer, FOREIGN KEY (package_id) REFERENCES packages(id));"
#define CREATE_ASSET_DEP_TABLE      "CREATE TABLE content_dependencies(parent_id int, child_id int, PRIMARY KEY (parent_id, child_id));"

MaelstromProject::MaelstromProject()
{
    this->synched = true;
}

MaelstromProject::~MaelstromProject()
{
    QSqlDatabase db = QSqlDatabase::database();
    if(db.isOpen())
        db.close();
    QSqlDatabase::removeDatabase(db.databaseName());
}

MaelstromProject *MaelstromProject::create(const QString& projectDirectory, const QString& projectName, bool createDirectory, const QString assetDbPath)
{
    MaelstromProject *project = new MaelstromProject();

    QDir dir(projectDirectory);
    if(createDirectory)
    {
        dir.mkdir(projectName);
        dir.cd(projectName);
    }

    project->setProjectPath(dir.absoluteFilePath(projectName + ".mae"));
    project->setName(projectName);
    project->setAssetDbPath(assetDbPath);
    project->setDatabaseDriver("QSQLITE"); // Hardcode sqlite driver for now

    project->save();

    QSqlDatabase db = QSqlDatabase::addDatabase(project->getDatabaseDriver());
    db.setDatabaseName(project->getAssetDbPath());
    db.open();

    return project;
}

MaelstromProject *MaelstromProject::open(const QString &path)
{
    MaelstromProject *project = new MaelstromProject();

    project->setProjectPath(path);

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QDataStream dataStream(&file);

    dataStream >> *project;

    QSqlDatabase db = QSqlDatabase::addDatabase(project->getDatabaseDriver());
    db.setDatabaseName(project->getAssetDbPath());
    db.open();

    return project;
}

QString MaelstromProject::getName() const
{
    return this->name;
}

void MaelstromProject::setName(const QString name)
{
    this->name = name;
    this->synched = false;
    emit this->nameChanged();
}

QString MaelstromProject::getAssetDbPath() const
{
    return this->assetDbPath;
}

QString MaelstromProject::getDatabaseDriver() const
{
    return this->databaseDriver;
}

void MaelstromProject::setAssetDbPath(const QString path)
{
    this->assetDbPath = path;
    this->synched = false;
}

void MaelstromProject::setDatabaseDriver(const QString driver)
{
    this->databaseDriver = driver;
}

QString MaelstromProject::getProjectPath() const
{
    return this->projectPath;
}

void MaelstromProject::setProjectPath(const QString path)
{
    this->projectPath = path;
    this->synched = false;
    emit this->projectPathChanged();
}

bool MaelstromProject::getInSync() const
{
    return this->synched;
}

void MaelstromProject::save(void)
{
    QFile file(this->projectPath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream dataStream(&file);
    dataStream << *this;
    this->synched = true;
}

void MaelstromProject::resetAssetDatabase()
{
    QSqlDatabase db = QSqlDatabase::database();
    if(db.isOpen())
    {
        db.close();
    }

    QFile file(this->assetDbPath);
    if(file.exists())
    {
        file.resize(0);
    }

    db.setDatabaseName(this->assetDbPath);
    db.open();

    QSqlQuery query;
    bool result;

    result = query.exec(CREATE_PACKAGE_TABLE);
    result = query.exec(CREATE_ASSET_TABLE);
    QSqlError error = query.lastError();
    QString message = error.text();
    result = query.exec(CREATE_ASSET_DEP_TABLE);
}

void MaelstromProject::addAssetPackage(const QString name)
{
    QString sql("INSERT INTO packages (name) VALUES ('" + name + "')");
    QSqlQuery query;
    query.exec(sql);
}

void MaelstromProject::deleteAssetPackage(const int id)
{
    QString sql("DELETE FROM packages WHERE id=" + id);
    QSqlQuery query;
    query.exec(sql);
}

QDataStream &operator<<(QDataStream &out, const MaelstromProject &project)
{
    out << project.getName();
    out << project.getAssetDbPath();
    out << project.getDatabaseDriver();
    return out;
}

QDataStream &operator>>(QDataStream &in, MaelstromProject &project)
{
    QString name;
    QString assetDbPath;
    QString databaseDriver;

    in >> name;
    in >> assetDbPath;
    in >> databaseDriver;

    project.setName(name);
    project.setAssetDbPath(assetDbPath);
    project.setDatabaseDriver(databaseDriver);
    return in;
}
