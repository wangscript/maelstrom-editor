#include <vacp_domain/include/asset.h>
#include <vacp_data/include/asset_data.h>

Asset::E_ERROR Asset::load_dep_branch(void)
{
	void *data = AssetDAL::getDependency(this->name);
	
	return E_OK;
}