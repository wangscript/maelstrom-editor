#include "build.h"
#include <asset.h>
#include <error.h>
#include <QFileInfo>
#include "pipeline.h"
#include <iostream>
#include <QTextCodec>
#include "output_color.h"

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
	Asset *asset;
	std::cout << "Resolving asset data... ";
	try
	{
		asset = Asset::get_by_name(this->asset);
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
    
    this->tree_depth = 0;

	std::cout << "Resolving dependency tree... ";
	try
	{
		this->init_dep_tree(0, asset);
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
		std::cout << std::endl << "> Building asset " << asset->get_name().toLocal8Bit().constData() << std::endl;
        ContentExporter *exporter = this->pipeline.get_texture_exporter(asset->get_exporter());
        QTextCodec *codec = QTextCodec::codecForLocale();
        QByteArray asset_source =  codec->fromUnicode(asset->get_source());
        char *source = asset_source.data();

		std::cout << "\tExporting using " << asset->get_exporter().toLocal8Bit().constData() << "... ";
        IContent *texture_data = exporter->process(source);
        if(texture_data == NULL)
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
		

        char *pixels = texture_data->get_pchar_value("PIXELDATA");

        ContentCompiler *compiler = this->pipeline.get_texture_compiler(asset->get_compiler());
        QByteArray asset_output = codec->fromUnicode(asset->get_output());
        QByteArray asset_config = codec->fromUnicode(asset->get_compileconfig());
        char *output = asset_output.data();
        char * config = asset_config.data();

		std::cout << "\tCompiling using " << asset->get_compiler().toLocal8Bit().constData() << "... ";
        int result = compiler->process(
                    texture_data,
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

        exporter->destroy(texture_data);
    }
    else
    {
        std::cout << "Asset up date. Nothing to do for: " << asset->get_name().toLocal8Bit().data() << std::endl;
    }
    delete asset;
}
