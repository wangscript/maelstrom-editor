#include <QtCore/QCoreApplication>
#include <iostream>
#include <pipeline.h>

#define BUILD_ACTOR	"ba"
#define BUILD_STATIC	"bs"


void printUsage(void)
{
    using namespace std;
    cout << "Vortex Asset Conditioning Pipeline" << endl;
    cout << "Usage: vacp [db] [command] [asset name]" << endl;
    cout << "[db]:\t\tpath to sqlite database." << endl;
    cout << "[command]:\t'ba' to build actor object." << endl;
    cout << "\t\t'bs' to build static object." << endl;
    cout << "[asset name]:\tname of asset to build" << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc != 4)
    {
        printUsage();
        exit(1);
    }
    else
    {
        QString cmd(argv[2]);
        QString db(argv[1]);
        QString asset(argv[3]);

        Pipeline *pipeline;
        try
        {
            pipeline = new Pipeline(db);
        }
        catch(std::exception &ex)
        {
            std::cerr << ex.what() << std::endl;
            exit(1);
        }

        if(cmd.compare(BUILD_ACTOR) == 0)
        {
            try
            {
                pipeline->build_actor(asset);
            }
            catch(std::exception &ex)
            {
                std::cerr << ex.what() << std::endl;
                exit(1);
            }
        }
        else if(cmd.compare(BUILD_STATIC) == 0)
        {
        }
        else
        {

            std::cerr << "Invalid command '" << cmd.data() << std::endl;
        }

        pipeline->destroy();

    }
    return a.exec();
}
