#include "output_color.h"
#include <QtCore/QCoreApplication>
#include <iostream>
#include <pipeline.h>
#include <stdlib.h>
#include <QMap>
#include <QStringList>

#define BUILD_ACTOR	"ba"
#define BUILD_STATIC	"bs"
#define BUILD_PACKAGE   "pk"
#define NO_FLAGS          4

typedef struct
{
    QString flag_text;
    int req_no_args;
} flag;

flag flags[NO_FLAGS] =
{
    {QString("-db"), 1},
    {QString("-in"), 1},
    {QString("-out"), 1},
    {QString("-norebuild"), 0}
};


void printUsage(void)
{
    using namespace std;
    cout << "Usage: vacp [command] [db] [asset name]" << endl;
    cout << "[db]:\t\tpath to sqlite database." << endl;
    cout << "[command]:\t'ba' to build actor object." << endl;
    cout << "\t\t'bs' to build static object." << endl;
    cout << "\t\t'pk' to build asset package." << endl;
    cout << "[asset name]:\tname of asset to build" << endl;
}

void parse_flags(QStringList &args, QMap<QString, QStringList*> *flag_map)
{
    bool flag_found;
    for(int i = 2; i < args.count(); i++)
    {
        flag_found = false;
        for(int j = 0; j < NO_FLAGS; j++)
        {
            if(args.at(i) == flags[j].flag_text)
            {
                flag_found = true;
                if(i + flags[j].req_no_args >= args.count())
                {
                    std::cerr << "Not enough arguments supplied for flag: " << flags[j].flag_text.toLocal8Bit().constData() << std::endl;
                    exit(1);
                }

                QStringList *flag_args = NULL;
                if(flags[j].req_no_args > 0)
                {
                    flag_args = new QStringList;
                    for(int k = 0; k < flags[j].req_no_args; k++)
                    {
                        flag_args->push_back(QString(args.at(++i)));
                    }
                }

                flag_map->insert(flags[j].flag_text, flag_args);

                break;
            }
        }

        if(!flag_found)
        {
            std::cerr << "Unknown flag specified: " << args.at(i).toLocal8Bit().constData() << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    set_color(CLR_WHITE);

    std::cout << "Vortex Asset Conditioning Pipeline" << std::endl << std::endl;

    if(argc < 4)
    {
        printUsage();
    }
    else
    {
        QString cmd(argv[1]);

        QMap<QString, QStringList*> *flags = new QMap<QString, QStringList*>();
        QStringList args = a.arguments();

        //parse_flags(args, flags);
        parse_flags(args, flags);

        Pipeline *pipeline;
        try
        {
            QString database = flags->value(QString("-db"))->at(0);
            pipeline = new Pipeline(database);
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
                QString asset = flags->value(QString("-in"))->at(0);
                QString out     = flags->value("-out")->at(0);
                std::cout << "Build Actor: " << asset.toLocal8Bit().constData() << std::endl;
                pipeline->build_actor(asset, out);
            }
            catch(std::exception &ex)
            {
                std::cerr << ex.what() << std::endl;
                exit(1);
            }
        }
        else if(cmd.compare(BUILD_STATIC) == 0)
        {
            // Not implemented yet.
        }
        else if(cmd.compare(BUILD_PACKAGE) == 0)
        {
            try
            {
                QString package = flags->value("-in")->at(0);
                QString out     = flags->value("-out")->at(0);
                std::cout << "Build Package: " << package.toLocal8Bit().constData() << " to " << out.toLocal8Bit().constData() << std::endl;
                pipeline->build_package(package, out);
                pipeline->link_package(package, out);

            }
            catch(std::exception &ex)
            {
                std::cerr << ex.what() << std::endl;
            }
        }
        else
        {
            std::cerr << "Invalid command '" << cmd.toLocal8Bit().data() << "'" << std::endl;
        }

        pipeline->destroy();

    }
    return 0;
}
