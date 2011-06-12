#include <pipeline.h>
#include <error.h>
#include <QSqlDatabase>
#include <QsqlError>
#include <QDir>
#include <QLibrary>

void context_callback(Pipeline &pl, BuildContext *ctx)
{
    pl.context_finished(ctx);
}

Pipeline::Pipeline(QString &db_path)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_path);
    if(!db.open())
    {
        QSqlError error = db.lastError();
        QString message(error.text());
        throw new DBException(message);
    }

    this->scan_plugins();
}

void Pipeline::build_actor(QString& asset)
{
    BuildContext *ctx = new BuildContext(asset, context_callback);
    this->ctxs.push_back(ctx);

    ctx->exec();
}

void Pipeline::scan_plugins(void)
{
    QDir dir("/plugins");
    if(!dir.exists())
    {
        QDir::current().mkdir("plugins");
    }

    QStringList dir_entries = dir.entryList();
    for(int i = 0; i < dir_entries.size(); i++)
    {
        if(QLibrary::isLibrary(dir_entries.at(i)))
        {
            QLibrary lib(dir_entries.at(i));
            lib.resolve("");
        }
    }
}

void Pipeline::destroy(void)
{
    QSqlDatabase::database().close();
    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
}

void Pipeline::context_finished(BuildContext *ctx)
{
    this->ctxs.remove(ctx);
    delete ctx;
}
