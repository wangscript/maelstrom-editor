#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H
#include <QVariant>

class ITexture
{
public:
    virtual qint32 *get_data() = 0;
};

class TextureData
{
private:
    qint32 *data;
    qint32 size;
    qint32 stride;
public:
    TextureData(qint32 *texture_data, qint32 data_size, qint32 texture_stride) :
        data(texture_data),
        size(data_size),
        stride(texture_stride)
    {
    }

    qint32 *get_data()
    {
        return this->data;
    }

    qint32 get_stride()
    {
        return this->stride;
    }

    qint32 get_size()
    {
        return this->size;
    }
};

#endif // TEXTUREDATA_H
