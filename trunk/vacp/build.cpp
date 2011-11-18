#include "build.h"
#include <content.h>
#include <package.h>
#include <error.h>
#include <QFileInfo>
#include "pipeline.h"
#include <iostream>
#include <QTextCodec>
#include "output_color.h"
#include <QList>
#include <QDir>
#include <QtXml/QDomDocument>
#include <QTextStream>
#include <QDateTime>
#include <utilities.h>

BuildContext::BuildContext(Pipeline &pipeline, ContentInfo &asset, QString &build_dir, build_callback callback)
    :
      parent_pipeline(pipeline),
      context_content(asset),
      build_dir(build_dir),
      cbk(callback)
{
}

void BuildContext::exec()
{
    this->tree_depth = 0;

    std::cout << "Resolving dependency tree... ";
    try
    {
        this->init_dep_tree(0, this->context_content);
        set_color(CLR_GREEN);
        std::cout << "OK" << std::endl;
        set_color(CLR_WHITE);
    }
    catch(std::exception &ex)
    {
        set_color(CLR_RED);
        std::cout << "FAIL" << std::endl;
        set_color(CLR_WHITE);
        std::cout << ex.what() << std::endl;
    }

    std::cout << "Building dependency tree... ";
    try
    {
        this->build_tree();
        std::cout << std::endl << "> Build succeeded." << std::endl;
    }
    catch(std::exception &ex)
    {
        std::cout << std::endl << "> Build failed." << std::endl;
        std::cout << ex.what() << std::endl;
    }
}

void BuildContext::create_meta(ContentInfo &content)
{
    QDir *dir = this->get_output_path(content);
    QString meta_output = dir->filePath(content.get_name() + ".contentmeta");
    delete dir;

    QList<ContentInfo*> *deps = content.get_dependencies();

    QDomDocument document("ContentMetaML");
    QDomElement root = document.createElement("ContentMeta");
    document.appendChild(root);

    QDomElement deps_parent_element = document.createElement("Dependencies");
    root.appendChild(deps_parent_element);
    for(int i = 0; i < deps->count(); i++)
    {
        QDomElement deps_element = document.createElement("Dependency");
        deps_element.setAttribute("Name", deps->at(i)->get_name_hash());
        deps_element.setAttribute("Package", Utilities::calc_str_hash(deps->at(i)->get_package_name()->toLocal8Bit().data()));
        deps_parent_element.appendChild(deps_element);
    }

    QFile meta_file(meta_output);
    if(!meta_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        throw new IOException("Unable to create metadata file: " + meta_output);
    }

    QTextStream text_stream(&meta_file);
    text_stream << document.toString();

    meta_file.close();
}

void BuildContext::init_dep_tree(int depth, ContentInfo &root_asset)
{
    if(this->tree_depth < depth)
        this->tree_depth = depth;

    this->dep_map.insert(depth, &root_asset);
    QList<ContentInfo*> *deps = root_asset.get_dependencies();

    QList<ContentInfo*>::iterator it;
    for(it = deps->begin(); it < deps->end(); it++)
    {
        this->init_dep_tree(depth + 1, *(*it));
    }
}

void BuildContext::build_tree(void)
{
    for(int i = this->tree_depth; i >= 0; i--)
    {
        QList<ContentInfo*> values = this->dep_map.values(i);
        for(int j = 0; j < values.size(); j++)
        {
            ContentInfo *asset = values.at(j);
            this->build_content(asset);
        }
    }
}

QDir *BuildContext::get_output_path(ContentInfo &asset) const
{
    QDir *output_dir = new QDir(this->build_dir);
    QString *package_name = asset.get_package_name();
    if(!output_dir->cd(*package_name))
    {
        if(!(output_dir->mkdir(*package_name) && output_dir->cd(*package_name)))
        {
            throw new IOException("Unable to create folder '" + *package_name + " in " + output_dir->absolutePath());
        }
    }

    return output_dir;
}

ContentInfo& BuildContext::get_input_asset() const
{
    return this->context_content;
}

void BuildContext::build_content(ContentInfo *asset)
{
    QDir *dir = this->get_output_path(*asset);
    QString build_output = dir->filePath(asset->get_name() + ".content");
    delete dir;

    bool should_build = asset->get_force_build();

    if(!should_build)
        should_build = asset->get_output().isEmpty() ||
                !QFile::exists(build_output);

    if(!should_build)
    {
        QFileInfo info(asset->get_source());
        should_build = (info.lastModified() > asset->get_last_built());
    }

    if(should_build)
    {

        std::cout << std::endl << "> Building asset " << asset->get_name().toLocal8Bit().constData() << std::endl;
        ContentExporter *exporter = this->parent_pipeline.get_texture_exporter(asset->get_exporter());
        QTextCodec *codec = QTextCodec::codecForLocale();
        QByteArray asset_source =  codec->fromUnicode(asset->get_source());

        QString test("Hello ö");
        QByteArray asd = test.toLatin1();
        int len = asd.length();
        char *source = asset_source.data();

        std::cout << "\tExporting using " << asset->get_exporter().toLocal8Bit().constData() << "... ";
        IContent *content_data = exporter->process(source);
        if(content_data == NULL)
        {
            set_color(CLR_RED);
            std::cout << "FAIL" << std::endl;
            set_color(CLR_WHITE);
            QString message(exporter->get_last_error_msg().c_str());
            throw new ExportFailureException(message);
            std::cerr << "\t" << exporter->get_last_error_msg() << std::endl << std::endl;
        }
        else
        {
            set_color(CLR_GREEN);
            std::cout << "OK" << std::endl;
            set_color(CLR_WHITE);
        }


        //char *pixels = texture_data->get_pchar_value("PIXELDATA");

        ContentCompiler *compiler = this->parent_pipeline.get_texture_compiler(asset->get_compiler());
        QByteArray asset_output = codec->fromUnicode(build_output);
        QByteArray asset_config = codec->fromUnicode(asset->get_compileconfig());
        char *output = asset_output.data();
        char * config = asset_config.data();

        std::cout << "\tCompiling using " << asset->get_compiler().toLocal8Bit().constData() << "... ";
        int result = compiler->process(
                    content_data,
                    output,
                    config);

        if(result != 0)
        {
            set_color(CLR_RED);
            std::cout << "FAIL" << std::endl;
            set_color(CLR_WHITE);
            std::cout << "\t" << compiler->get_last_error_msg() << std::endl << std::endl;
        }
        else
        {
            set_color(CLR_GREEN);
            std::cout << "OK" << std::endl;
            set_color(CLR_WHITE);
        }

        exporter->destroy(content_data);
    }
    else
    {
        std::cout << "Asset up date. Nothing to do for: " << asset->get_name().toLocal8Bit().data() << std::endl;
    }

    this->create_meta(*asset);
}
