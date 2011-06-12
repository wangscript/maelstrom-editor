#ifndef CONTENTEXPORTER_H
#define CONTENTEXPORTER_H

template <class T>
class ContentExporter
{
public:
    ContentExporter()
    {
    }

    virtual T* process(char *path) = 0;
};

#endif // CONTENTEXPORTER_H
