#ifndef BUILD_H
#define BUILD_H

#include <QMultiMap>


class ContentInfo;
class Pipeline;
class QDir;
class QString;

class BuildContext
{
public:
        typedef void (*build_callback)(Pipeline &pl, BuildContext*);
    BuildContext(Pipeline &pipeline, ContentInfo &content, QString &build_dir, build_callback callback);
        void exec();
        ContentInfo& get_input_asset() const;
private:
        Pipeline &parent_pipeline;
        ContentInfo &context_content;
        QString &build_dir;
        build_callback cbk;
        int tree_depth;

        QMultiMap<unsigned int, ContentInfo*> dep_map;

        void build_content(ContentInfo*);
        void build_tree();
        void create_meta(ContentInfo&);
        void init_dep_tree(int, ContentInfo&);
        QDir *get_output_path(ContentInfo&) const;
};

#endif // BUILD_H
