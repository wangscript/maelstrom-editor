#include "pluginmanager.h"
#include <QDir>
#include <QLibrary>
#include <iostream>

typedef void (*plugin_register_delegate)(PluginManager&);

PluginManager::PluginManager()
{
}

void PluginManager::register_plugins(void)
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
            plugin_register_delegate dlg = (plugin_register_delegate)lib.resolve("register_plugin");
            if(dlg != NULL)
            {
                dlg(*this);
            }
        }
    }
}

void PluginManager::register_exporter(char *exporter_name)
{
    QMap<char*, QLibrary*>::iterator it;
    if(this->exporters.find(exporter_name) == this->exporters.end())
    {
        std::cerr << "Multiple registrations of exporter '" << exporter_name << "'. Ignoring subsequent registrations for this exporter";
    }
    else
    {
        this->exporters.insert(exporter_name, NULL);
    }

}

void PluginManager::register_compiler(char *compiler_name)
{
}
