#define EXPORTER_NAME	"Bmp Texture Exporter"

#include <Windows.h>
#include <iostream>
#include "C:\Projects\maelstrom-engine\maelstrom-editor\vacp\plugin_header\vacp_Plugin_common.h"
extern "C"
{
	class TextureData : ITextureData
	{
	private:
		unsigned char *data;
		unsigned int length;
		unsigned int stride;
	public:
		TextureData(unsigned char *data, unsigned int length, unsigned int stride)
			: data(data), length(length), stride(stride)
		{
		}

		virtual unsigned char *get_data(void)
		{
			return this->data;
		}

		virtual unsigned int get_data_length(void)
		{
			return this->length;
		}

		virtual unsigned int get_stride(void)
		{
			return this->stride;
		}
	};

	class BmpExporter : public ContentExporter<TextureData>
	{
		virtual TextureData *process(char *path)
		{
			unsigned char *data = (unsigned char*)malloc(255);
			for(int i = 0; i < 255; i++)
			{
				data[i] = static_cast<unsigned char>(i);
			}

			return new TextureData(data, 255, 50);
		}

		virtual void destroy(TextureData *data)
		{
		}
	};

	__declspec(dllexport) BmpExporter *create(void)
	{
		return new BmpExporter;
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