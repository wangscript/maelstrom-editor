#define NO_PLUGIN_HEADER
#define EXPORTER_NAME_LIMIT     255

#include "pluginmanager.h"
#include <QDir>
#include <QLibrary>
#include <iostream>
#include <QDebug>
#include <Windows.h>
#include <plugin_header/vacp_plugin_common.h>

typedef void (*plugin_get_exporter_name)(char*, unsigned int);
typedef void (*plugin_register_delegate)(PluginManager&);
typedef ContentExporter<ITextureData> *(*plugin_get_texture_exporter)(void);

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
                plugin_get_exporter_name get_exporter_name =
                        static_cast<plugin_get_exporter_name>(lib->resolve("get_exporter_name"));

                if(!get_exporter_name)
                {
                    std::cerr << "Library is not valid plugin. Can not find symbol get_exporter_name: " << dir_entries.at(i).fileName().toLocal8Bit().constData();
                    lib->unload();
                    delete lib;
                }
                else
                {
                    char *exporter_name = new char[EXPORTER_NAME_LIMIT];
                    get_exporter_name(exporter_name, EXPORTER_NAME_LIMIT);

                    QMap<char*, QLibrary*>::iterator it = this->exporters.find(exporter_name);
                    if(it != this->exporters.end())
                    {
                        std::cerr << "Exporter '" << exporter_name << "' in "
                                  << lib->fileName().toLocal8Bit().constData() << " has already been registered by "
                                  << it.value()->fileName().toLocal8Bit().constData() << std::endl;
                        lib->unload();
                        delete lib;
                    }
                    else
                    {
                        this->exporters.insert(exporter_name, lib);
                        lib->unload();
                    }
                }
            }
        }
    }
}

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
    if(this->compilers.find(compiler_name) == this->exporters.end())
    {
        std::cerr << "Multiple registrations of exporter '" << compiler_name << "'. Ignoring subsequent registrations for this exporter";
    }
    else
    {
        QLibrary *lib = static_cast<QLibrary*>(token);
        this->compilers.insert(compiler_name, lib);
    }
}
