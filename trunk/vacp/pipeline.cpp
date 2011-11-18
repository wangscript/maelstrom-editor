#include <pipeline.h>
#include <error.h>
#include <pluginmanager.h>
#include <QSqlDatabase>
#include <QSqlError>
#include "output_color.h"
#include "Package.h"
#include <QDir>
#include "packageelement.h"
#include <QList>
#include <QDomDocument>
#include <QtXmlPatterns/QXmlQuery>

class ContentInfo;

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

void Pipeline::build_actor(QString& asset, QString& build_dir)
{
    //BuildContext *ctx = new BuildContext(*this, asset, build_dir, BuildContext::BUILD_ASSET, context_callback);
    //this->ctxs.push_back(ctx);

    //ctx->exec();
}

QList<BuildContext*> *Pipeline::build_package_internal(Package &package, QString &build_dir)
{
    QList<BuildContext*> *contexts = new QList<BuildContext*>;

    // Get all assets in package.
    QList<ContentInfo*> *package_assets = package.get_contentinfo();

    // Create and execute all build contexts.
    for(int i = 0; i < package_assets->count(); i++)
    {
        BuildContext *ctx = new BuildContext(*this, *package_assets->at(i), build_dir, context_callback);
        contexts->push_back(ctx);
        ctx->exec();
    }

    delete package_assets;

    return contexts;
}


void Pipeline::build_package(QString& package_name, QString& build_dir)
{
    Package *package;
    std::cout << "Resolving package data... ";

    package = Package::get_by_name(package_name);
    set_color(CLR_GREEN);
    std::cout << "OK" << std::endl;
    set_color(CLR_WHITE);



    // Create output directory if it does not exist: {$build}\package_name
    QDir dir(build_dir);
    if(!dir.cd(package->get_name()))
    {
        if(!(dir.mkdir(package->get_name()) && dir.cd(package->get_name())))
        {
            throw new IOException("Unable to create folder '" + package->get_name() + "in " + build_dir);
        }
    }

    // When writing content metadata, do I need the list of BuildContexts? Or should I skip the metadata files?
    QList<BuildContext*> *contexts = this->build_package_internal(*package, build_dir);

    // Since all BuildContexts execute synchronously, its safe to delete them at this point.
    // this will not be the case if rewritten to be asynchronous.
    for(int i = 0; i < contexts->count(); i++)
    {

        BuildContext *ctx = contexts->at(i);
        delete ctx;
    }

    delete contexts;
    delete package;
}

void Pipeline::link_package(QString &package_name, QString &build_dir)
{
    Package *package;

    package = Package::get_by_name(package_name);

    QDir dir(build_dir);
    if(!dir.cd(package_name))
    {
        QString message("Build directory does not exist.");
        throw new DirectoryAccessException(message);
    }

    QString package_path = dir.filePath(package_name + ".pak");
    QFile package_file(package_path);
    if(!package_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        delete package;
        throw new IOException("Could not open or create '" + package_path + "'");
    }

    QList<ContentInfo*> *assets = package->get_contentinfo();

    quint32 table_size = 4 + (assets->count() * 8); // 4 initial bytes + 8 bytes per content.


    // Resize to fit the package table, and skip past it since we dont have enough information to compose it yet.
    if(!package_file.resize(table_size) || !package_file.seek(table_size))
    {
        for(int i = 0; i < assets->count(); i++)
        {
            delete assets->at(i);
        }
        delete assets;
        delete package;

        throw new IOException("Could not access '" + package_path + "'");
    }

    QDataStream package_stream(&package_file);
    QList<PackageElement*> elements;

    package_stream.setByteOrder(QDataStream::LittleEndian);

    // Read all content files and simply copy the data over to the package file.

    for(int i = 0; i < assets->count(); i++)
    {
        QString content_path = dir.filePath(assets->at(i)->get_name() + ".content");
        QString meta_path = dir.filePath(assets->at(i)->get_name() + ".contentmeta");

        QFile content_file(content_path);
        if(!content_file.open(QIODevice::ReadOnly))
        {
            // Perform cleanup.
            throw new IOException("Expected file '" + content_path + "' does not exist. Has the package been built?");
        }

        QFile meta_file(meta_path);
        if(!meta_file.open(QIODevice::ReadOnly))
        {
            // Perform cleanup.
            throw new IOException("Expected file '" + meta_path + "' does not exist. Has the package been built?");
        }

        QDomDocument meta_document;
        if(!meta_document.setContent(&meta_file))
        {
            // Perform cleanup
            throw new IOException("Unable to read content meta file '" + meta_path + "'");
        }

        QXmlQuery query;
        QString querystr = "ContentMeta/Dependencies/Dependency";
        QUrl url(meta_path);

        query.setQuery(querystr);
        //QStringList list;
        //query.evaluateTo(&list);

        /*QDomNode meta_node = root.firstChild();
        while(!meta_node.isNull())
        {
            QDomElement meta_element = meta_node.toElement();
            if(!meta_element.isNull() && meta_element.tagName() == "Dependency")
            {

            }
        }

        QDomElement root = meta_document.documentElement();
        if(root.tagName() != "ContentMetaML")
        {
            // File is not a content meta file!
            // Throw some kind of exception!
        }*/




        quint64 content_size = content_file.size();
        elements.push_back(new PackageElement(
                               assets->at(i)->get_id(),
                               content_size + 4)); // 4 is added to content_size to take into account the leading size integer.


        QDataStream content_stream(&content_file);

        char *content_raw_buffer = new char[content_size];

        content_stream.readRawData(content_raw_buffer, content_size);

        package_stream << (quint32)content_size;
        package_stream << (char)0;
        package_stream.writeBytes(content_raw_buffer, content_size);
        delete[] content_raw_buffer;

        content_file.close();
    }

    // Move to the beginning of the file, so we can write what we skipped earlier: The package table.

    package_file.seek(0);
    quint32 offset = table_size;

    package_stream << (quint32)elements.count();
    for(int i = 0; i < elements.count(); i++)
    {
        package_stream << elements.at(i)->get_hash()
        package_stream << offset;

        offset += elements.at(i)->get_size();
    }
    package_file.close();

    // Cleanup.

    for(int i = 0; i < elements.count(); i++)
    {
        delete elements.at(i);
    }

    for(int i = 0; i < assets->count(); i++)
    {
        delete assets->at(i);
    }
    delete assets;
    delete package;
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
