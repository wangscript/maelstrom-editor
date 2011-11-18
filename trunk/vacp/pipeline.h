#ifndef PIPELINE_H
#define PIPELINE_H

#include <QMap>
#include <content.h>
#include <build.h>
#include <QList>

#define NO_PLUGIN_HEADER
#include <plugin_header/vacp_plugin_common.h>

class BuildContext;
class Package;
class PluginManager;
class QString;


class Pipeline
{
private:
    QMap<QString*, ContentExporter*> texture_exporters;
    QMap<QString*, ContentCompiler*> texture_compilers;
    PluginManager *plugin_manager;
    std::list<BuildContext*> ctxs;

    QList<BuildContext*> *build_package_internal(Package&, QString&);
public:
    Pipeline(QString&);
    void build_actor(QString&, QString&);
    void build_package(QString&, QString&);
    void context_finished(BuildContext *ctx);
    void destroy();
    void link_package(QString&, QString&);

    ContentExporter *get_texture_exporter(QString &exporter_name);
    ContentCompiler *get_texture_compiler(QString &compiler_name);
};

#endif // PIPELINE_H
