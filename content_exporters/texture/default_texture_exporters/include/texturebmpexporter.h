#ifndef TEXTUREBMPEXPORTER_H
#define TEXTUREBMPEXPORTER_H

#if defined(__linux)
#include <stdint.h>
typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int 		U32;
typedef signed int		I32;

#define PACKED(definition) \
	definition __attribute__((packed));
#endif
#if defined(WIN32)
typedef unsigned __int8		U8;
typedef unsigned __int16	U16;
typedef unsigned __int32	U32;
typedef __int32			I32;

#define PACKED(definition) \
	__pragma(pack(push, 1)) \
	definition; \
	__pragma(pack(pop))

#endif

#include <fstream>
#include <../../../../vacp/plugin_header/vacp_plugin_common.h>

PACKED(
struct BITMAP_FILEHEADER {
		U16			bfType;
        U32			bfSize;
        U16			bfReserved1;
        U16			bfReserved2;
        U32			bfOffBits;
})

PACKED(
struct BITMAP_INFOHEADER{
        U32			biSize;
        I32			biWidth;
        I32			biHeight;
        U16			biPlanes;
        U16			biBitCount;
        U32			biCompression;
        U32			biSizeImage;
        I32			biXPelsPerMeter;
        I32			biYPelsPerMeter;
        U32			biClrUsed;
        U32			biClrImportant;
})

PACKED(
struct BITMAP_V3HEADER
{
		U32		        bV4Size;
        I32				bV4Width;
        I32				bV4Height;
        U16		        bV4Planes;
        U16		        bV4BitCount;
        U32		        bV4V4Compression;
        U32		        bV4SizeImage;
        I32				bV4XPelsPerMeter;
        I32				bV4YPelsPerMeter;
        U32		        bV4ClrUsed;
        U32		        bV4ClrImportant;
        U32		        bV4RedMask;
        U32		        bV4GreenMask;
        U32		        bV4BlueMask;
        U32		        bV4AlphaMask;
})

enum BITMAP_COMPRESSION
{
	CMP_RGB,
	CMP_RLE8,
	CMP_RLE4,
	CMP_BITFIELDS,
	CMP_JPEG,
	CMP_PNG
};

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
	char *last_error;
	int parse_dib_bitmapinfoheader(std::ifstream &input, BITMAP_INFOHEADER &header);
public:
	BmpExporter(void);
	virtual Content *process(char *path);

	virtual void destroy(Content *data);
};

#endif
