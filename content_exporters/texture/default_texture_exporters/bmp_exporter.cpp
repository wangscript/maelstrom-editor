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
			int row_size = ceil((dib_header.biBitCount * dib_header.biWidth) / 32.0f) * 4;
		
			char *pixel_data = new char[dib_header.biSizeImage];
			input.seekg(header.bfOffBits, std::ios_base::beg);
			input.read(pixel_data, dib_header.biSizeImage);

			// When height is non-negavite, its scan lines are flipped vertically so we must flip them.
			if(dib_header.biHeight > 0)
			{
				char *pixel_data_temp = new char[dib_header.biSizeImage];
				int rows = dib_header.biSizeImage / row_size;

				// Copy all scanline rows into temp buffer in reverse order
				for(int y = 0; y < rows; y++)
					memcpy(	static_cast<void*>(&pixel_data_temp[y * row_size]),
							static_cast<void*>(&pixel_data[(rows - y - 1) * row_size]),
							row_size);

				// Copy entire temp buffer content into main buffer.
				memcpy(	static_cast<void*>(pixel_data),
						static_cast<void*>(pixel_data_temp),
						dib_header.biSizeImage);

				delete[] pixel_data_temp;
			}

			texture->set_pchar_value("PIXELDATA", pixel_data);
			texture->set_int_value("PIXELDATA_LEN", new int(dib_header.biSizeImage));
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
