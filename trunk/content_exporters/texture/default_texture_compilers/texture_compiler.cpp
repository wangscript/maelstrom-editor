#include <fstream>
#define COMPILER_NAME	"Texture compiler"
#include <textureddscompiler.h>
#include <sstream>
#include <string>
#include <iostream>
#define DX10_PIXELFORMAT	808540228UL
#define CONFIG_MIPMAP		std::string("Mipmap")
#define CONFIG_COMPRESSION	std::string("Compression")

#if defined(WIN32)
#define EXPORT	__declspec(dllexport)
#endif

#if defined(__linux)
#define EXPORT
#endif

extern "C"
{
	int TextureDDSCompiler::prepare_dds_header(DDS_HEADER &header, Content *input)
	{
		memset(&header, 0, sizeof(DDS_HEADER));

		header.dwSize = 124;
		header.dwHeaderFlags = 
			DDS_HEADER_FLAGS_TEXTURE;

		int *width = input->get_int_value("WIDTH");
		int *height = input->get_int_value("HEIGHT");
		if(!width)
		{
			std::string msg("Content does not contain expected key WIDTH.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}

		if(!height)
		{
			std::string msg("Content does not contain expected key HEIGHT.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}
		header.dwHeight	= *height;
		header.dwWidth = *width;
		std::cout << "PreCHECK mipmap" << std::endl;
		std::string mipmap = CONFIG_MIPMAP;
		std::string *mipmap_value = ContentCompiler::get_config(mipmap);
		if(!mipmap_value)
		{
			std::cout << "CHECK mipmap" << std::endl;
			std::string msg("No mipmap configuration was supplied.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}
		else
		{
			std::cout << "Fleh mipmap" << std::endl;
			try
			{
				header.dwMipMapCount = ContentCompiler::str_to_int(*mipmap_value);
				header.dwHeaderFlags |= DDS_HEADER_FLAGS_MIPMAP;
			}
			catch(std::exception &ex)
			{
				std::cout << "mipmap fail" << std::endl;
				std::string msg("Invalid mipmap value.");
				ContentPlugin::set_last_error_msg(msg);
				return -1;
			}
		}

		//DDS_PIXELFORMAT pf;
		//memset(&pf, 0, sizeof(DDS_PIXELFORMAT));
		std::cout << "Checking compression" << std::endl;
		std::string compression = CONFIG_COMPRESSION;
		std::string *compression_value = ContentCompiler::get_config(compression);
		if(!compression_value)
		{
			std::cout << "compression fail" << std::endl;
			std::string msg("No compression configuration was supplied.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}
		else
		{
			std::cout << "compression nonfail " << *compression_value << std::endl;
			if(compression_value->compare("None") == 0)
			{
				std::cout << "No compression." << std::endl;
				header.ddspf = DDSPF_A8R8G8B8;
			}
			else if(compression_value->compare("DXT1") == 0)
			{
				header.ddspf = DDSPF_DXT1;
			}
			else if(compression_value->compare("DXT2") == 0)
			{
				header.ddspf = DDSPF_DXT2;
			}
			else if(compression_value->compare("DXT3") == 0)
			{
				header.ddspf = DDSPF_DXT3;
			}
			else if(compression_value->compare("DXT4") == 0)
			{
				header.ddspf = DDSPF_DXT4;
			}
			else if(compression_value->compare("DXT5") == 0)
			{
				header.ddspf = DDSPF_DXT5;
			}
			else if(compression_value->compare("DX10") == 0)
			{
				header.ddspf = DDSPF_DX10;
			}
			else
			{
				std::string msg("Invalid compression value.");
				ContentPlugin::set_last_error_msg(msg);
				return -1;
			}
		}
		//header.ddspf = pf;

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
		std::cout << header.dwSize << "|" << header.dwHeaderFlags << "|" << header.dwHeight << "|" << header.dwWidth << "|" << header.dwPitchOrLinearSize << "|"<< header.dwDepth << "|" << header.dwMipMapCount << std::endl;
		std::cout << "ddspf" << "|" << header.ddspf.dwSize << "|" << header.ddspf.dwFlags << "|" << header.ddspf.dwFourCC << "|" << header.ddspf.dwRGBBitCount << "|" << header.ddspf.dwRBitMask << std::endl;
		output.write(reinterpret_cast<char*>(&header), sizeof(DDS_HEADER));
	}

	int TextureDDSCompiler::process(Content *input, const char *path, char *config)
	{
		ContentCompiler::parse_config(config);

		std::ofstream output(path, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
		if(!output.good())
		{
			std::string msg("Unable to open or create output file.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}

		DDS_HEADER header;

		if(prepare_dds_header(header, input) != 0)
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
		if(!data)
		{
			std::string msg("Content does not contain expected key PIXELDATA.");
			ContentPlugin::set_last_error_msg(msg);
			output.close();
			return -1;
		}


		int *data_len = input->get_int_value("PIXELDATA_LEN");
		if(!data_len)
		{
			std::string msg("Content does not contain expected key PIXELDATA_LEN.");
			ContentPlugin::set_last_error_msg(msg);
			output.close();
			return -1;
		}

		output.write(data, *data_len);

		char *header_d = reinterpret_cast<char*>(&header);
		std::cout << "LOLe" << std::endl;
		for(int i = 0; i < 124; i++)
			std::cout << header_d[i];
		std::cout << std::endl << "eLOL" << std::endl;
		output.close();

		return 0;
	}

	class TextureDDSCompilerFactory : public ContentCompilerFactory
	{
	public:
		TextureDDSCompilerFactory() : ContentCompilerFactory(
			"DDS Texture Compiler",
			"Mipmap;int;false;Compression;list;None,DXT1,DXT2,DXT3,DXT4,DXT5")
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

	EXPORT ContentCompilerFactory *create_compiler_factory(void)
	{
		return new TextureDDSCompilerFactory;
	}
}
