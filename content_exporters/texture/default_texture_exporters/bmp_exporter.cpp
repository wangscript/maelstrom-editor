#define EXPORTER_NAME	"Bmp Texture Exporter"

#include <iostream>
#include <fstream>
#include <list>
#include <texturebmpexporter.h>
#include <stdlib.h>
#include <cmath>
#include <cstring>
#include <algorithm>


#define BUFFER_SIZE			512

#define BITMAPSIGNATURE			19778

#define BITMAPCOREHEADER_SIZE	12
#define BITMAPCOREHEADER2_SIZE	64
#define BITMAPINFOHEADER_SIZE	40
#define BITMAPV2INFOHEADER_SIZE	52
#define BITMAPV3INFOHEADER_SIZE	56
#define BITMAPV4HEADER_SIZE		108
#define BITMAPV5HEADER_SIZE		124

#if defined(WIN32)
#include <Windows.h>
#define EXPORT __declspec(dllexport)
#endif

#if defined(__linux)
#define EXPORT
#endif

extern "C"
{
	BmpExporter::BmpExporter()
	{
		this->last_error = NULL;
	}
	IContent *BmpExporter::process(char *path)
	{

		std::ifstream input(path, std::ifstream::binary);
		if(!input.good())
		{
			std::string msg("Could not open file for processing.");
			msg.append(path);
			ContentPlugin::set_last_error_msg(msg);
			return NULL;
		}

		Content *texture = new Content;

		BITMAP_FILEHEADER header;

		input.read(reinterpret_cast<char*>(&header), sizeof(BITMAP_FILEHEADER));
		if(header.bfType != BITMAPSIGNATURE)
		{
			input.close();
			std::string msg("Input is not a valid Bitmap Image File.");
			ContentPlugin::set_last_error_msg(msg);
			return NULL;
		}

		I32 dib_header_size;
		input.read(reinterpret_cast<char*>(&dib_header_size), sizeof(I32));
		input.seekg(-4, std::ios_base::cur);

		if(dib_header_size == BITMAPINFOHEADER_SIZE)
		{
			BITMAP_INFOHEADER dib_header;
			if(this->parse_dib_bitmapinfoheader(input, dib_header) != 0)
			{
				input.close();
				return NULL;
			}

			texture->set_int_value("WIDTH", new int(dib_header.biWidth));
			texture->set_int_value("HEIGHT", new int(dib_header.biHeight));

			if(dib_header.biBitCount <= 8)
			{
				int* palette = new int[dib_header.biClrUsed];
				input.read(reinterpret_cast<char*>(palette), sizeof(palette));
			}
			int stride = ceil((dib_header.biBitCount * dib_header.biWidth) / 32.0f) * 4;
			int stride_padding = stride - ((dib_header.biBitCount / 8) * dib_header.biWidth);

			char *pixel_data = new char[dib_header.biSizeImage];
			input.seekg(header.bfOffBits, std::ios_base::beg);
			input.read(pixel_data, dib_header.biSizeImage);

			// When height is non-negavite, its scan lines are flipped vertically so we must flip them.
			if(dib_header.biHeight > 0)
			{
				//this->flip_scanlines(pixel_data, dib_header.biSizeImage, dib_header.biSizeImage / row_size);
			}

			// Every texture exporter must export texture data in 32BPP, so we need to transform the bitmap data if it is not 32BPP.
			char *transformed_pixel_data;
			U32 transformed_length;
			switch(dib_header.biBitCount)
			{
			case 24:
				this->transform_24BPP(pixel_data, stride, stride_padding, dib_header.biHeight, &transformed_pixel_data, &transformed_length);
				//this->transform_24BPP(pixel_data, dib_header.biSizeImage, dib_header.biSizeImage / row_size, &transformed_pixel_data, &transformed_length);
				delete[] pixel_data;
				pixel_data = transformed_pixel_data;
				break;
			default:
				transformed_length = dib_header.biSizeImage;
				break;
			}

			texture->set_pchar_value("PIXELDATA", pixel_data);
			texture->set_int_value("PIXELDATA_LEN", new int(transformed_length));
		}
		else if(dib_header_size == BITMAPV3INFOHEADER_SIZE)
		{
			BITMAP_V3HEADER dib_header;
			input.read(reinterpret_cast<char*>(&dib_header), sizeof(dib_header));
			char *pixel_data = new char[dib_header.bV4SizeImage];
			input.seekg(header.bfOffBits, std::ios_base::beg);
			input.read(pixel_data, dib_header.bV4SizeImage);

			texture->set_pchar_value("PIXELDATA", pixel_data);
			texture->set_int_value("PIXELDATA_LEN", new int(dib_header.bV4SizeImage));
		}
		else
		{
			input.close();
			std::string msg("Unkown DIB header size.");
			ContentPlugin::set_last_error_msg(msg);
			return NULL;
		}
		input.close();

		char *pdat = texture->get_pchar_value("PIXELDATA");
		return texture;
	}

	void BmpExporter::flip_scanlines(char *data, U32 size, U32 stride)
	{
		char *pixel_data_temp = new char[size];
		int rows = size / stride;

		// Copy all scanline rows into temp buffer in reverse order
		for(int y = 0; y < rows; y++)
			memcpy(	static_cast<void*>(&pixel_data_temp[y * stride]),
			static_cast<void*>(&data[(rows - y - 1) * stride]),
			stride);

		// Copy entire temp buffer content into main buffer.
		memcpy(	static_cast<void*>(data),
			static_cast<void*>(pixel_data_temp),
			size);

		delete[] pixel_data_temp;
	}

	void BmpExporter::transform_24BPP(char *data, U32 stride, U32 stride_padding, U32 scanlines, char **transformed_data, U32 *transformed_length)
	{
		U32 pixels_per_scanline = (stride - stride_padding) / 3;
		U32 total_pixels = pixels_per_scanline * scanlines;

		U32 new_stride = pixels_per_scanline * 4;

		*transformed_length = total_pixels * 4;

		*transformed_data = new char[*transformed_length];
		for(int y = 0; y < scanlines; y++)
		{
			for(int x = 0; x < pixels_per_scanline; x++)
			{
				int source_x = x*3;
				int source_y = stride*y;
				int dest_x = x*4;
				int dest_y = new_stride*y;

				char a = 0xFF;
				char b = data[(source_y) + (source_x)];
				char g = data[(source_y) + (source_x) + 1];
				char r = data[(source_y) + (source_x) + 2];
				(*transformed_data)[(dest_y) + (dest_x) + 0] = b; // Fully opaque
				(*transformed_data)[(dest_y) + (dest_x) + 1] = g;
                (*transformed_data)[(dest_y) + (dest_x) + 2] = r;
				(*transformed_data)[(dest_y) + (dest_x) + 3] = a;
			}
		}
	}

	int BmpExporter::parse_dib_bitmapinfoheader(std::ifstream &input, BITMAP_INFOHEADER &header)
	{
		input.read(reinterpret_cast<char*>(&header), sizeof(header));
		if(header.biCompression != CMP_RGB)
		{
			std::string msg("Bitmap compression is not supported.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}

		return 0;
	}


	void BmpExporter::destroy(IContent *data)
	{

	}

	class BmpExporterFactory : public ContentExporterFactory
	{
	public:
		BmpExporterFactory() : ContentExporterFactory("Bmp Texture Exporter", "")
		{
		}

		BmpExporter *create(void)
		{
			return new BmpExporter;
		}

		void destroy(ContentExporter *exporter)
		{
			delete exporter;
		}
	};

	EXPORT ContentExporterFactory *create_exporter_factory(void)
	{
		return new BmpExporterFactory;
	}
}

#if defined(WIN32)
BOOL WINAPI DllMain(
	__in  HINSTANCE hinstDLL,
	__in  DWORD fdwReason,
	__in  LPVOID lpvReserved
	)
{
	return TRUE;
}
#endif


//extern "C" __declspec(dllexport) register_plugin
