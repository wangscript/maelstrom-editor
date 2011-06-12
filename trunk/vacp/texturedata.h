#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H
#include <QVariant>

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

    qint32 *get_data(void)
    {
        return this->data;
    }

    qint32 get_stride(void)
    {
        return this->stride;
    }

    qint32 get_size(void)
    {
        return this->size;
    }
};

#endif // TEXTUREDATA_H
