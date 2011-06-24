#include <fstream>
#define COMPILER_NAME	"Texture compiler"
#include <textureddscompiler.h>
#include <sstream>
#include <string>

#define DX10_PIXELFORMAT	808540228UL
#define CONFIG_MIPMAP		std::string("Generate mipmap")
#define CONFIG_COMPRESSION	std::string("Compression")

extern "C"
{
	int TextureDDSCompiler::prepare_dds_header(DDS_HEADER &header, Content *input)
	{
		header.dwSize = 124;
		header.dwHeaderFlags =
			DDS_HEADER_FLAGS_TEXTURE;

		std::string *mipmap_value = ContentCompiler::get_config(CONFIG_MIPMAP);
		if(!mipmap_value)
		{
			return -1;
		}
		else
		{
			try
			{
				header.dwMipMapCount = ContentCompiler::str_to_int(*mipmap_value);
				header.dwHeaderFlags |= DDS_HEADER_FLAGS_MIPMAP;
			}
			catch(std::exception &ex)
			{
				return -1;
			}
		}

		DDS_PIXELFORMAT pf;
		std::string *compression_value = ContentCompiler::get_config(CONFIG_COMPRESSION);
		if(!compression_value)
		{
			return -1;
		}
		else
		{
			if(compression_value->compare("None") == 0)
			{
				pf = DDSPF_A8R8G8B8;
			}
			else if(compression_value->compare("DXT1") == 0)
			{
				pf = DDSPF_DXT1;
			}
			else if(compression_value->compare("DXT2") == 0)
			{
				pf = DDSPF_DXT2;
			}
			else if(compression_value->compare("DXT3") == 0)
			{
				pf = DDSPF_DXT3;
			}
			else if(compression_value->compare("DXT4") == 0)
			{
				pf = DDSPF_DXT4;
			}
			else if(compression_value->compare("DXT5") == 0)
			{
				pf = DDSPF_DXT5;
			}
			else if(compression_value->compare("DX10") == 0)
			{
				pf = DDSPF_DX10;
			}
			else
			{
				return -1;
			}
		}
		header.ddspf = pf;

		header.dwHeight	= *input->get_int_value("HEIGHT");
		header.dwWidth = *input->get_int_value("WIDTH");

		return 0;
	}

	void TextureDDSCompiler::serialize_dds_header(DDS_HEADER& header, std::ofstream& output)
	{
		/*output << DDS_MAGIC;
		output << header.dwSize;
		output << header.dwHeaderFlags;
		output << header.dwHeight;
		output << header.dwWidth;
		output << header.dwPitchOrLinearSize;
		output << header.dwDepth;
		output << header.dwMipMapCount;
		for(int i = 0; i < 11; i++)
			output << header.dwReserved1[i];
		output << header.ddspf.dwSize;
		output << header.ddspf.dwFlags;
		output << header.ddspf.dwFourCC;
		output << header.ddspf.dwRGBBitCount;
		output << header.ddspf.dwRBitMask;
		output << header.ddspf.dwGBitMask;
		output << header.ddspf.dwBBitMask;
		output << header.ddspf.dwABitMask;
		output << header.dwSurfaceFlags; //dwCaps on MSDN documentation
		output << header.dwCubemapFlags; // dwCaps2 on MSDN documentation;
		for(int i = 0; i < 3; i++)
			output << header.dwReserved2[i];*/
		output.write(reinterpret_cast<char*>(&header), sizeof(DDS_HEADER));
	}

	int TextureDDSCompiler::process(Content *input, const char *path, char *config)
	{
		ContentCompiler::parse_config(config);

		std::ofstream output(path, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
		if(!output.good())
			return -1;

		DDS_HEADER header;

		if(!prepare_dds_header(header, input))
		{
			output.close();
			return -1;
		}

		serialize_dds_header(header, output);

		if(header.ddspf.dwFourCC == DX10_PIXELFORMAT)
		{
			// CREATE AND SERIALIZE DX10 HEADER
		}

		char *data = input->get_pchar_value("PIXELDATA");
		int *data_len = input->get_int_value("PIXELDATA_LEN");
		if(!data_len)
		{
			output.close();
			return -1;
		}

		output.write(data, *data_len);

		output.close();

		return 0;
	}

	class TextureDDSCompilerFactory : public ContentCompilerFactory
	{
	public:
		TextureDDSCompilerFactory() : ContentCompilerFactory(
			"DDS Texture Compiler",
			"Generate mipmap;bool;false;Compression;list;None,DXT1,DXT2,DXT3,DXT4,DXT5")
		{
		}

		TextureDDSCompiler *create(void)
		{
			return new TextureDDSCompiler;
		}

		void destroy(ContentCompiler *compiler)
		{
			delete compiler;
		}
	};

	__declspec(dllexport) ContentCompilerFactory *create_compiler_factory(void)
	{
		return new TextureDDSCompilerFactory;
	}
}