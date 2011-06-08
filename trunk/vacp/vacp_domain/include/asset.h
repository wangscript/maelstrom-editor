#ifndef ASSET_H
#define ASSET_H

#include <string>
#include <vector>

class Asset
{
private:
	std::string &name;
public:
	enum E_ERROR
	{
		E_OK,
		E_NO_SUCH_ASSET
	};
	std::vector<Asset*> *pAssetDeps;
	E_ERROR load_dep_branch(void);

	Asset(std::string &asset) : name(asset) {}
};
#endif