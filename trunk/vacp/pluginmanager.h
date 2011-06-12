#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QMap>

class QLibrary;

class PluginManager
{
private:
    QMap<char*, QLibrary*> exporters;
    QMap<char*, QLibrary*> compilers;
public:
    void register_plugins(void);
    void register_exporter(char *exporter_name);
    void register_compiler(char *compiler_name);
    PluginManager();
};

#endif // PLUGINMANAGER_H
