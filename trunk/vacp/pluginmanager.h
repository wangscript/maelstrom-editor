#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QMap>

#define NO_PLUGIN_HEADER
#include <plugin_header/vacp_plugin_common.h>

class QLibrary;
class ContentExporter;
class ContentExporterFactory;

class PluginManager
{
private:
    QMap<QString, ContentExporterFactory*> exporters;
    QMap<QString, ContentCompilerFactory*> compilers;
public:
    void register_plugins(void);

    void register_exporter(char *exporter_name, void *token);
    void register_compiler(char *compiler_name, void *token);

    ContentExporter *create_texture_exporter(QString &exporter_name);
    ContentCompiler *create_texture_compiler(QString &compiler_name);
    PluginManager();
};

#endif // PLUGINMANAGER_H
