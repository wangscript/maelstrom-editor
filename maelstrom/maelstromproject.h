#ifndef MAELSTROMPROJECT_H
#define MAELSTROMPROJECT_H

#include <QObject>
#include <QString>

class MaelstromProject : public QObject
{
   Q_OBJECT

private:
    QString name;
    QString projectPath;
    QString assetDbPath;
    QString databaseDriver;
    bool synched;
    MaelstromProject();
public:
    static MaelstromProject *create(const QString& projectDirectory, const QString& projectName, bool createDirectory, const QString assetDbPath);
    static MaelstromProject *open(const QString&);
    QString getAssetDbPath() const;
    QString getDatabaseDriver() const;
    bool getInSync() const;
    QString getName() const;
    QString getProjectPath() const;
    void resetAssetDatabase();
    void setAssetDbPath(const QString);
    void setDatabaseDriver(const QString);
    void setName(const QString);
    void setProjectPath(const QString);
    void save();
    void addAssetPackage(const QString name);
    void deleteAssetPackage(const int id);
    ~MaelstromProject();
signals:
    void nameChanged();
    void projectPathChanged();
};

QDataStream &operator<<(QDataStream &out, const MaelstromProject &project);
QDataStream &operator>>(QDataStream &in, MaelstromProject &project);

#endif // MAELSTROMPROJECT_H
