#include "build.h"
#include <asset.h>
#include <error.h>
#include <QFileInfo>

BuildContext::BuildContext(QString &asset_name, build_callback callback)
    :       asset(asset_name),
      cbk(callback)
{

}

void BuildContext::init_dep_tree(unsigned int depth, Asset *root_asset)
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
    for(unsigned int i = this->tree_depth; i >= 0; i--)
    {
        QList<Asset*> values = this->dep_map.values(i);
        for(int j = 0; j < values.size(); j++)
        {
            this->build_asset(values.at(j));
        }
    }
}

void BuildContext::build_asset(Asset* asset)
{
    // Build asset if no output file exists
    bool should_build = asset->get_output().isEmpty() ||
            !QFile::exists(asset->get_output());

    if(!should_build)
    {
        QFileInfo info(asset->get_source());
        should_build = (info.lastModified() > asset->get_last_built());
    }

    if(should_build)
    {

    }
    delete &asset;
}
