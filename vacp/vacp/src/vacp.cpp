#include <iostream>
#include <string>
#include <vacp_domain/include/asset.h>

#define BUILD_ACTOR		"ba"
#define BUILD_STATIC	"bs"

using namespace std;


void printInfo(void)
{
	cout << "Vortex Asset Conditioning Pipeline" << endl;
	cout << "Usage: vacp [db] [command] [asset name]" << endl;
	cout << "[db]:\t\tpath to sqlite database." << endl;
	cout << "[command]:\t'ba' to build actor object." << endl;
	cout << "\t\t'bs' to build static object." << endl;
	cout << "[asset name]:\tname of asset to build" << endl;
}

int buildActor(const char *db, const char *asset)
{
	Asset::E_ERROR res;
	string assetname(asset);
	Asset *pAsset = new Asset(assetname);

	res = pAsset->load_dep_branch();
	return 0;
}

int buildStatic(const char *db, const char *asset)
{
	return 0;
}

int main(int argc, char **argv)
{
	int result;
	if(argc < 3)
	{
		printInfo();
		result = -1;
	}
	else
	{
		if(strstr(argv[2], BUILD_ACTOR) != NULL)
		{
			result = buildActor(argv[1], argv[3]);
		}
		else if(strstr(argv[2], BUILD_STATIC) != NULL)
		{
			result = buildStatic(argv[1], argv[3]);
		}
		else
		{
			printInfo();
			result = -1;
		}
		
	}
	return result;
}