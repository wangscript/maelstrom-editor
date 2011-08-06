#ifndef PIPELINE_H
#define PIPELINE_H
#include <QString>
#include <QMap>
#include <asset.h>
#include <string>
#include <list>
#include <build.h>

#define NO_PLUGIN_HEADER
#include <plugin_header/vacp_plugin_common.h>

class PluginManager;

class Pipeline
{
private:
    QMap<QString*, ContentExporter*> texture_exporters;
    QMap<QString*, ContentCompiler*> texture_compilers;
    PluginManager *plugin_manager;
    std::list<BuildContext*> ctxs;
public:
    Pipeline(QString&);
    void build_actor(QString&);
    void destroy(void);
    void context_finished(BuildContext *ctx);

    ContentExporter *get_texture_exporter(QString &exporter_name);
    ContentCompiler *get_texture_compiler(QString &compiler_name);
};

#endif // PIPELINE_H
