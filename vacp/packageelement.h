#ifndef PACKAGEELEMENT_H
#define PACKAGEELEMENT_H

class Package;
class ContentInfo;

class PackageElement
{
private:
    quint32 hash;
    quint32 size;
public:
    PackageElement(quint32 name_hash, quint32 content_size)
        : hash(name_hash),
          size(content_size)
    {

    }

    quint32 get_hash()
    {
        return this->hash;
    }

    quint32 get_size()
    {
        return this->size;
    }
};

#endif // PACKAGEELEMENT_H
