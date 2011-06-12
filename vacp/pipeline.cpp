#include <pipeline.h>
#include <error.h>
#include <QSqlDatabase>
#include <QsqlError>

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
}

void Pipeline::build_actor(QString& asset)
{
        BuildContext *ctx = new BuildContext(asset, context_callback);
        this->ctxs.push_back(ctx);

        ctx->exec();
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
