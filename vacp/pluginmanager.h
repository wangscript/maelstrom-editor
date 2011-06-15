#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QMap>

class QLibrary;

template <class T>
class ContentExporter;

class PluginManager
{
private:
    QMap<char*, QLibrary*> exporters;
    QMap<char*, QLibrary*> compilers;
public:
    void register_plugins(void);

    template <class T>
    void register_content_exporter(ContentExporter<T> &exporter);

    void register_exporter(char *exporter_name, void *token);
    void register_compiler(char *compiler_name, void *token);
    PluginManager();
};

#endif // PLUGINMANAGER_H
