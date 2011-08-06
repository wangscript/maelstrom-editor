#include <pipeline.h>
#include <error.h>
#include <pluginmanager.h>
#include <QSqlDatabase>
#include <QSqlError>3
#include "output_color.h"

void context_callback(Pipeline &pl, BuildContext *ctx)
{
    pl.context_finished(ctx);
}

Pipeline::Pipeline(QString &db_path)
{
	std::cout << "Connecting to database...";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_path);
    if(!db.open())
    {
		set_color(CLR_RED);
		std::cout << "FAIL" << std::endl;
		set_color(CLR_WHITE);
        QSqlError error = db.lastError();
        QString message(error.text());
        throw new DBException(message);
    }
	else
	{
		set_color(CLR_GREEN);
		std::cout << "OK" << std::endl;
		set_color(CLR_WHITE);
	}

	std::cout << "Registering plugins..." << std::endl;
    this->plugin_manager = new PluginManager;
    this->plugin_manager->register_plugins();
}

void Pipeline::build_actor(QString& asset)
{
    BuildContext *ctx = new BuildContext(*this, asset, context_callback);
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

ContentExporter *Pipeline::get_texture_exporter(QString &exporter_name)
{
    ContentExporter *exporter = NULL;
    QMap<QString*, ContentExporter* >::iterator it = this->texture_exporters.find(&exporter_name);
    if(it == this->texture_exporters.end())
    {
        exporter = this->plugin_manager->create_texture_exporter(exporter_name);
        QString *key = new QString(exporter_name);
        this->texture_exporters.insert(key, exporter);
    }
    else
    {
        exporter = it.value();
    }

    return exporter;
}

ContentCompiler *Pipeline::get_texture_compiler(QString &compiler_name)
{
    ContentCompiler *compiler = NULL;
    QMap<QString*, ContentCompiler*>::iterator it = this->texture_compilers.find(&compiler_name);
    if(it == this->texture_compilers.end())
    {
        compiler = this->plugin_manager->create_texture_compiler(compiler_name);
        QString *key = new QString(compiler_name);
        this->texture_compilers.insert(key, compiler);
    }
    else
    {
        compiler = it.value();
    }

    return compiler;
}
