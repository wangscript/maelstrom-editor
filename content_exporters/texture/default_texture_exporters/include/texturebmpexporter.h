#ifndef TEXTUREBMPEXPORTER_H
#define TEXTUREBMPEXPORTER_H

#include <fstream>
#include "C:\Projects\maelstrom-engine\maelstrom-editor\vacp\plugin_header\vacp_Plugin_common.h"

typedef struct tag_BITMAPFILEHEADER {
        unsigned __int16		bfType;
        unsigned __int32		bfSize;
        unsigned __int16		bfReserved1;
        unsigned __int16		bfReserved2;
        unsigned __int32		bfOffBits;
} BITMAP_FILEHEADER;

typedef struct tag_BITMAPINFOHEADER{
        unsigned __int32		biSize;
        __int32					biWidth;
        __int32					biHeight;
        unsigned __int16		biPlanes;
        unsigned __int16		biBitCount;
        unsigned __int32		biCompression;
        unsigned __int32		biSizeImage;
        __int32					biXPelsPerMeter;
        __int32					biYPelsPerMeter;
        unsigned __int32		biClrUsed;
        unsigned __int32		biClrImportant;
} BITMAP_INFOHEADER;

typedef struct tag_BITMAPV3HEADER
{
	    unsigned __int32        bV4Size;
        __int32					bV4Width;
        __int32					bV4Height;
        unsigned __int16        bV4Planes;
        unsigned __int16        bV4BitCount;
        unsigned __int32        bV4V4Compression;
        unsigned __int32        bV4SizeImage;
        __int32					bV4XPelsPerMeter;
        __int32					bV4YPelsPerMeter;
        unsigned __int32        bV4ClrUsed;
        unsigned __int32        bV4ClrImportant;
        unsigned __int32        bV4RedMask;
        unsigned __int32        bV4GreenMask;
        unsigned __int32        bV4BlueMask;
        unsigned __int32        bV4AlphaMask;
} BITMAP_V3HEADER;

class TextureData : public ITextureData
{
private:
	char *data;
	unsigned int length;
	unsigned int stride;
	unsigned int width;
	unsigned int height;
public:
	TextureData() {}
	TextureData(char *data, unsigned int length, unsigned int stride);

	virtual char *get_data(void);
	void set_data(char *data);

	virtual unsigned int get_data_length(void);
	void set_data_length(unsigned int length);

	virtual unsigned int get_stride(void);
	void set_stride(unsigned int stride);

	virtual int get_width(void);
	void set_width(int width);

	virtual int get_height(void);
	void set_height(int height);
};

class BmpExporter : public ContentExporter
{
private:
	int parse_dib_bitmapinfoheader(std::ifstream &input, BITMAP_INFOHEADER &header);
public:
	virtual Content *process(char *path);

	virtual void destroy(Content *data);

};

typedef struct
{
	__int32				width;
	__int32				height;
	unsigned __int16	color_planes;
	unsigned __int16	bpp;
	unsigned __int32	compression_method;
	unsigned __int32	image_size;
	__int32				horizontal_res;
	__int32				vertical_res;
	unsigned __int32	no_colors;
	unsigned __int32	no_important_colors;
} BITMAPINFOHEADER_FLEH;

#endif