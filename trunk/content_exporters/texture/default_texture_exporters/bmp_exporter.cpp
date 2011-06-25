#define EXPORTER_NAME	"Bmp Texture Exporter"

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <list>
#include <texturebmpexporter.h>
#include <stdlib.h>


#define BUFFER_SIZE			512

#define BITMAPSIGNATURE			19778

#define BITMAPCOREHEADER_SIZE	12
#define BITMAPCOREHEADER2_SIZE	64
#define BITMAPINFOHEADER_SIZE	40
#define BITMAPV2INFOHEADER_SIZE	52
#define BITMAPV3INFOHEADER_SIZE	56
#define BITMAPV4HEADER_SIZE		108
#define BITMAPV5HEADER_SIZE		124

extern "C"
{
	TextureData::TextureData(char *data, unsigned int length, unsigned int stride)
		: data(data), length(length), stride(stride)
	{
	}

	char *TextureData::get_data(void)
	{
		return this->data;
	}

	void TextureData::set_data(char *data)
	{
		this->data = data;
	}

	unsigned int TextureData::get_data_length(void)
	{
		return this->length;
	}

	void TextureData::set_data_length(unsigned int length)
	{
		this->length = length;
	}

	unsigned int TextureData::get_stride(void)
	{
		return this->stride;
	}

	void TextureData::set_stride(unsigned int stride)
	{
		this->stride = stride;
	}

	int TextureData::get_width(void)
	{
		return this->width;
	}

	void TextureData::set_width(int width)
	{
		this->width = width;
	}

	int TextureData::get_height(void)
	{
		return this->height;
	}

	void TextureData::set_height(int height)
	{
		this->height = height;
	}

	Content *BmpExporter::process(char *path)
	{
		std::ifstream input(path, std::ifstream::binary);
		if(!input.good())
			return NULL;

		Content *texture = new Content;

		BITMAPFILEHEADER header;

		input.read(reinterpret_cast<char*>(&header), sizeof(BITMAPFILEHEADER));
		if(header.bfType != BITMAPSIGNATURE)
		{
			input.close();
			return NULL;
		}

		__int32 dib_header_size;
		input.read(reinterpret_cast<char*>(&dib_header_size), sizeof(__int32));
		input.seekg(-4, std::ios_base::cur);
		if(dib_header_size == BITMAPINFOHEADER_SIZE)
		{
			BITMAPINFOHEADER dib_header;
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
			BITMAPV3HEADER dib_header;
			input.read(reinterpret_cast<char*>(&dib_header), sizeof(dib_header));
			char *pixel_data = new char[dib_header.bV4SizeImage];
			input.seekg(header.bfOffBits, std::ios_base::beg);
			input.read(pixel_data, dib_header.bV4SizeImage);

			texture->set_pchar_value("PIXELDATA", pixel_data);
			texture->set_int_value("PIXELDATA_LEN", new int(dib_header.bV4SizeImage));
		}

		return texture;
	}

	int BmpExporter::parse_dib_bitmapinfoheader(std::ifstream &input, BITMAPINFOHEADER &header)
	{
		input.read(reinterpret_cast<char*>(&header), sizeof(header));
		if(header.biCompression != BI_RGB)
			return -1;

		return 0;
	}


	void BmpExporter::destroy(Content *data)
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

	__declspec(dllexport) ContentExporterFactory *create_exporter_factory(void)
	{
		return new BmpExporterFactory;
	}
}

BOOL WINAPI DllMain(
	__in  HINSTANCE hinstDLL,
	__in  DWORD fdwReason,
	__in  LPVOID lpvReserved
	)
{
	return TRUE;
}


//extern "C" __declspec(dllexport) register_plugin
