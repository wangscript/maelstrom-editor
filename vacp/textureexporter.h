#ifndef TEXTUREEXPORTER_H
#define TEXTUREEXPORTER_H
#include <contentexporter.h>
#include <texturedata.h>

class TextureExporter : ContentExporter<TextureData>
{
public:
    TextureExporter();

    virtual TextureData *process(char *path)
    {
        TextureData *data = new TextureData(0, 0, 0);
        return data;
    }
};

#endif // TEXTUREEXPORTER_H
