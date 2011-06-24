#define EXPORTER_NAME_LIMIT     255

#include "pluginmanager.h"
#include <QDir>
#include <QLibrary>
#include <iostream>
#include <QDebug>
#include <Windows.h>
#include "error.h"

typedef void (*plugin_get_exporter_name)(char*, unsigned int);
typedef void (*plugin_register_delegate)(PluginManager&);
typedef ContentExporter *(*plugin_get_texture_exporter)(void);

typedef ContentExporterFactory *(plugin_create_content_exporter_factory)(void);
typedef ContentCompilerFactory *(plugin_create_content_compiler_factory)(void);

PluginManager::PluginManager()
{
}

void PluginManager::register_plugins(void)
{
    QDir plugin_dir = QDir::current();
    bool result = plugin_dir.cd("plugins");
    QString data = plugin_dir.path();
    if(!plugin_dir.exists())
    {
        QDir::current().mkdir("plugins");
    }

    const QFileInfoList dir_entries = plugin_dir.entryInfoList();
    for(int i = 0; i < dir_entries.size(); i++)
    {
        if(QLibrary::isLibrary(dir_entries.at(i).absoluteFilePath()))
        {
            QLibrary *lib = new QLibrary(dir_entries.at(i).absoluteFilePath());
            if(!lib->load())
            {
                std::cerr << "Failed to load plugin: " << dir_entries.at(i).fileName().toLocal8Bit().constData() << std::endl;
                lib->unload();
                delete lib;
            }
            else
            {
                plugin_create_content_exporter_factory *create_content_exporter_factory =
                        static_cast<plugin_create_content_exporter_factory*>(lib->resolve("create_exporter_factory"));

                plugin_create_content_compiler_factory *create_content_compiler_factory =
                        static_cast<plugin_create_content_compiler_factory*>(lib->resolve("create_compiler_factory"));

                if(!create_content_exporter_factory && !create_content_compiler_factory)
                {
                    std::cerr << dir_entries.at(i).fileName().toLocal8Bit().data() <<  ": Invalid plugin. Does not export create_compiler_factory nor create_exporter_factory" << std::endl;
                }

                if(create_content_compiler_factory)
                {
                    ContentCompilerFactory *factory = create_content_compiler_factory();
                    char *compiler_name = factory->get_name();

                    QMap<QString, ContentCompilerFactory*>::iterator it = this->compilers.find(compiler_name);
                    if(it != this->compilers.end())
                    {
                        std::cerr << "Compiler '" << compiler_name << "' in"
                                  << lib->fileName().toLocal8Bit().constData() << " has already been registered." << std::endl;
                        delete factory;
                        lib->unload();
                        delete lib;
                    }
                    else
                    {
                        this->compilers.insert(compiler_name, factory);
                    }
                }

                if(create_content_exporter_factory)
                {
                    ContentExporterFactory *factory = create_content_exporter_factory();
                    char *exporter_name = factory->get_name();

                    QMap<QString, ContentExporterFactory*>::iterator it = this->exporters.find(exporter_name);
                    if(it != this->exporters.end())
                    {
                        std::cerr << "Exporter '" << exporter_name << "' in "
                                  << lib->fileName().toLocal8Bit().constData() << " has already been registered." << std::endl;
                        delete factory;
                        lib->unload();
                        delete lib;
                    }
                    else
                    {
                        this->exporters.insert(exporter_name, factory);
                    }
                }
            }
        }
    }
}

/*
void PluginManager::register_exporter(char *exporter_name, void *token)
{
    QMap<char*, QLibrary*>::iterator it;
    if(this->exporters.find(exporter_name) == this->exporters.end())
    {
        std::cerr << "Multiple registrations of exporter '" << exporter_name << "'. Ignoring subsequent registrations for this exporter";
    }
    else
    {
        QLibrary *lib = static_cast<QLibrary*>(token);
        this->exporters.insert(exporter_name, lib);
    }
}

void PluginManager::register_compiler(char *compiler_name, void *token)
{
    QMap<char*, QLibrary*>::iterator it;
    if(this->compilers.find(compiler_name) == this->compilers.end())
    {
        std::cerr << "Multiple registrations of exporter '" << compiler_name << "'. Ignoring subsequent registrations for this exporter";
    }
    else
    {
        QLibrary *lib = static_cast<QLibrary*>(token);
        this->compilers.insert(compiler_name, lib);
    }
}
*/

ContentExporter *PluginManager::create_texture_exporter(QString &exporter_name)
{
    ContentExporter *exporter = NULL;
    QMap<QString, ContentExporterFactory*>::iterator it = this->exporters.find(exporter_name);
    if(it == this->exporters.end())
    {
        QString msg("An asset refers to an exporter that does not exist: ");
        msg.append(exporter_name);
        throw new InvalidAssetException(msg);
    }
    else
    {
        ContentExporterFactory *factory = it.value();
        exporter = factory->create();
    }

    return exporter;
}

ContentCompiler *PluginManager::create_texture_compiler(QString &compiler_name)
{
    ContentCompiler *compiler = NULL;
    QMap<QString, ContentCompilerFactory* >::iterator it = this->compilers.find(compiler_name);
    if(it == this->compilers.end())
    {
        QString msg("An asset refers to a compiler that does not exist: ");
        msg.append(compiler_name);
        // TODO: Should not throw this kind of exception.
        throw new InvalidAssetException(msg);
    }
    else
    {
        ContentCompilerFactory *factory = it.value();
        compiler = factory->create();
    }

    return compiler;
}
