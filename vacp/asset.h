#ifndef ASSET_H
#define ASSET_H
#include <string>
#include <vector>
#include <QString>
#include <QDateTime>

class Asset
{
private:
        QString &name;
        QString &source;
        QString &output;
        QDateTime &last_built;
        int id;
        Asset(int asset_id, QString &asset_name, QString &asset_source, QDateTime &asset_last_built, QString &asset_output);
        void validate(void);
public:
        static Asset* get_by_name(QString &name);
        std::vector<Asset*> *get_dependencies(void);
        ~Asset();

        QString &get_source();
        QString &get_output();
        QDateTime &get_last_built();
};

#endif // ASSET_H
