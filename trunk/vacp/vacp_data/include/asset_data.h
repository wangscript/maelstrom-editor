#ifndef ASSET_DATA_H
#define ASSET_DATA_H

#include <string>

class AssetDAL
{
public:
	static void *getDependency(std::string&);
};
#endif