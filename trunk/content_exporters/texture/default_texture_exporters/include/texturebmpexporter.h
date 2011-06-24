#ifndef TEXTUREBMPEXPORTER_H
#define TEXTUREBMPEXPORTER_H

#include <fstream>
#include "C:\Projects\maelstrom-engine\maelstrom-editor\vacp\plugin_header\vacp_Plugin_common.h"

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
	int parse_dib_bitmapinfoheader(std::ifstream &input, BITMAPINFOHEADER &header);
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