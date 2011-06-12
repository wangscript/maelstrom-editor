#ifndef PIPELINE_H
#define PIPELINE_H
#include <QString>
#include <asset.h>
#include <string>
#include <list>
#include <build.h>

class Pipeline
{
private:
        std::list<BuildContext*> ctxs;

public:
        Pipeline(QString&);
        void build_actor(QString&);
        void destroy(void);
        void context_finished(BuildContext *ctx);
};

#endif // PIPELINE_H
