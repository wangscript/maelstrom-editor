#ifndef BUILD_H
#define BUILD_H

#include <QString>
#include <string>
#include <map>
#include <QMultiMap>

class Pipeline;
class Asset;

class BuildContext
{
public:
        typedef void (*build_callback)(Pipeline &pl, BuildContext*);
        BuildContext(Pipeline &pipeline, QString &asset_name, build_callback callback);
        void exec(void);
private:
        Pipeline &pipeline;
        build_callback cbk;
        QString asset;
        int tree_depth;
        QMultiMap<unsigned int, Asset*> dep_map;

        void init_dep_tree(int, Asset*);
        void build_tree(void);
        void build_asset(Asset*);
};

#endif // BUILD_H
