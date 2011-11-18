#include <fstream>

#include <basic_shader_exporter.h>
#include <stdint.h>

#if defined(WIN32)
#include <Windows.h>
#define EXPORT __declspec(dllexport)
#endif

#if defined(__linux)
#define EXPORT
#endif

extern "C"
{
	IContent *BasicShaderExporter::process(char *path)
	{
		std::ifstream input(path, std::ifstream::binary);
		if(!input.good())
		{
			std::string msg("Could not open file for processing.");
			msg.append(path);
			ContentPlugin::set_last_error_msg(msg);
			return NULL;
		}

		Content *shader = new Content;

		input.seekg(0, std::ios_base::end);

		U32 size = input.tellg();

		input.seekg(0, std::ios_base::beg);

		char *data = new char[size];
		input.read(data, size);

		shader->set_pchar_value("DATA", data);
		shader->set_uint_value("DATALEN", new U32(size));
		
		U32 path_len = strlen(path);
		char *input_path = new char[path_len + 1];
		strcpy(input_path, path);
		shader->set_pchar_value("PATH", input_path);

		return shader;
	}

	void BasicShaderExporter::destroy(IContent *content)
	{
		U32 *datalen = content->get_uint_value("DATALEN");
		char *data = content->get_pchar_value("DATA");
		char *input_path = content->get_pchar_value("PATH");

		delete input_path;
		delete datalen;
		delete[] data;
		delete content;
	}

	class BasicShaderExporterFactory : public ContentExporterFactory
	{
	public:
		BasicShaderExporterFactory() : ContentExporterFactory("Basic Shader Exporter", "")
		{
		}

		BasicShaderExporter *create()
		{
			return new BasicShaderExporter;
		}

		void destroy(ContentExporter *exporter)
		{
			delete exporter;
		}
	};


	EXPORT ContentExporterFactory *create_exporter_factory(void)
	{
		return new BasicShaderExporterFactory;
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
