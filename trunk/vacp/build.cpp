#include "build.h"
#include <asset.h>
#include <error.h>
#include <QFileInfo>
#include "pipeline.h"
#include <iostream>
#include <QTextCodec>

BuildContext::BuildContext(Pipeline &pipeline, QString &asset_name, build_callback callback)
    :       pipeline(pipeline),
      asset(asset_name),
      cbk(callback)
{

}

void BuildContext::init_dep_tree(int depth, Asset *root_asset)
{
    if(this->tree_depth < depth)
        this->tree_depth = depth;

    this->dep_map.insert(depth, root_asset);
    std::vector<Asset*> *deps = root_asset->get_dependencies();

    std::vector<Asset*>::iterator it;
    for(it = deps->begin(); it < deps->end(); it++)
    {
        this->init_dep_tree(depth + 1, (*it));
    }
}

void BuildContext::exec(void)
{
    Asset *asset = Asset::get_by_name(this->asset);
    this->tree_depth = 0;
    this->init_dep_tree(0, asset);
    this->build_tree();
}

void BuildContext::build_tree(void)
{
    for(int i = this->tree_depth; i >= 0; i--)
    {
        QList<Asset*> values = this->dep_map.values(i);
        for(int j = 0; j < values.size(); ++j)
        {
            Asset *ass2 = values.at(i);
            this->build_asset(ass2);
        }
    }
}

void BuildContext::build_asset(Asset *asset)
{
    // Build asset if no output file exists
    bool should_build = asset->get_force_build();

    if(!should_build)
        should_build = asset->get_output().isEmpty() ||
                !QFile::exists(asset->get_output());

    if(!should_build)
    {
        QFileInfo info(asset->get_source());
        should_build = (info.lastModified() > asset->get_last_built());
    }

    if(should_build)
    {
        ContentExporter *exporter = this->pipeline.get_texture_exporter(asset->get_exporter());
        QTextCodec *codec = QTextCodec::codecForLocale();
        QByteArray asset_source =  codec->fromUnicode(asset->get_source());
        char *source = asset_source.data();

        Content *texture_data = exporter->process(source);
        if(texture_data == NULL)
        {
            std::cout << exporter->get_last_error_msg() << std::endl;
        }
        char *pixels = texture_data->get_pchar_value("PIXELDATA");
        char c;
        for(int i = 0; i < 117; i++)
        {
            c = pixels[i];
        }

        int i = *texture_data->get_int_value("WIDTH");

        ContentCompiler *compiler = this->pipeline.get_texture_compiler(asset->get_compiler());
        QByteArray asset_output = codec->fromUnicode(asset->get_output());
        QByteArray asset_config = codec->fromUnicode(asset->get_compileconfig());
        char *output = asset_output.data();
        char * config = asset_config.data();
        int result = compiler->process(
                    texture_data,
                    output,
                    config);

        if(result != 0)
        {
            std::cout << compiler->get_last_error_msg() << std::endl;
        }

        exporter->destroy(texture_data);
    }
    else
    {
        std::cout << "Asset up date. Nothing to do for: " << asset->get_name().toLocal8Bit().data() << std::endl;
    }
    delete asset;
}
