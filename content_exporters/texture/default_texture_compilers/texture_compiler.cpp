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
	int TextureDDSCompiler::prepare_dds_header(DDS_HEADER &header, IContent *input)
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
		std::string mipmap = CONFIG_MIPMAP;
		std::string *mipmap_value = ContentCompiler::get_config(mipmap);
		if(!mipmap_value)
		{
			std::string msg("No mipmap configuration was supplied.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}
		else
		{
			try
			{
				I32 mipmap = ContentCompiler::str_to_int(*mipmap_value);
				if(mipmap)
				{
					if(this->is_power_of_two(header.dwHeight) && this->is_power_of_two(header.dwWidth))
					{
						U32 width_bit = this->get_set_bit(header.dwWidth);
						U32 height_bit = this->get_set_bit(header.dwHeight);
						U32 mipmap_levels = std::min(width_bit, height_bit);

						header.dwMipMapCount = mipmap_levels;
					}
					else
					{
						// handle non-power-of-two textures.
					}
				}
				else
				{
					header.dwMipMapCount = 1;
				}
				//header.dwHeaderFlags |= DDS_HEADER_FLAGS_MIPMAP;
			}
			catch(std::exception &ex)
			{
				std::string msg("Invalid mipmap value.");
				ContentPlugin::set_last_error_msg(msg);
				return -1;
			}
		}

		//DDS_PIXELFORMAT pf;
		//memset(&pf, 0, sizeof(DDS_PIXELFORMAT));
		std::string compression = CONFIG_COMPRESSION;
		std::string *compression_value = ContentCompiler::get_config(compression);
		if(!compression_value)
		{
			std::string msg("No compression configuration was supplied.");
			ContentPlugin::set_last_error_msg(msg);
			return -1;
		}
		else
		{
			if(compression_value->compare("None") == 0)
			{
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

		return 0;
	}

	bool TextureDDSCompiler::is_power_of_two(U32 value)
	{
		U32 max = (1 << 31);
		U32 v = 1;
		U32 bits = 0;

		// loop and a half...
		if(value & v)
			bits++;
		do
		{
			v <<= 1;
			if(value & v)
				bits++;
		}while(v != max);

		return bits == 1;
	}

	U32 TextureDDSCompiler::get_set_bit(U32 value)
	{
		U32 max = (1 << 31);
		U32 v = 1;
		U32 bits = 0;
		bool bit_found;
		do
		{
			bits++;
			bit_found = (value & v) != 0;
			v <<= 1;
		}while(!bit_found && v != max);

		return bits;
	}

	void TextureDDSCompiler::serialize_dds_header(DDS_HEADER& header, std::ofstream& output)
	{
		I32 magic = DDS_MAGIC;
		int beforep = output.tellp();
		output.write(reinterpret_cast<char*>(&magic), 4);
		int afterp = output.tellp();
		output.flush();
		output.write(reinterpret_cast<char*>(&header), sizeof(DDS_HEADER));
	}

	int TextureDDSCompiler::process(IContent *input, const char *path, char *config)
	{
		ContentCompiler::parse_config(config);
		std::ofstream output(path, std::ios::out | std::ios::trunc | std::ios::binary);
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

		mipmap_chain *mm_chain = generate_mipmap_chain(data, *data_len, header.dwHeight, header.dwWidth, header.dwMipMapCount);
		for(int l = 0; l <= header.dwMipMapCount; l++)
		{
			output.write(mm_chain->chain[l].data, mm_chain->chain[l].length);
		}
		destroy_mipmap_chain(mm_chain);

		output.close();

		return 0;
	}

	mipmap_chain *TextureDDSCompiler::generate_mipmap_chain(char *data, U32 data_length, U32 height, U32 width, U32 chain_length)
	{
		mipmap_chain *mm_chain = new mipmap_chain;
		mm_chain->length = chain_length;
		mm_chain->chain = new texture2d[chain_length];

		mm_chain->chain[0].data = new char[data_length];
		mm_chain->chain[0].length = data_length;
		memcpy(mm_chain->chain[0].data, data, data_length);

		U32 mipmap_width = width;
		U32 mipmap_height = height;

		U32 cur_width;
		U32 pre_width = width;

		U32 *cur_level;
		U32 *pre_level;

		U32 box[4];

		for(int l = 1; l <= chain_length; l++)
		{
			mipmap_width >>= 1;
			mipmap_height >>= 1;

			cur_width = mipmap_width;
			mm_chain->chain[l].length = mipmap_width * mipmap_height * 4;
			mm_chain->chain[l].data = new char[mm_chain->chain[l].length];
			cur_level = reinterpret_cast<U32*>(mm_chain->chain[l].data);
			pre_level = reinterpret_cast<U32*>(mm_chain->chain[l-1].data);

			for(int y = 0; y < std::max((U32)1, mipmap_height); y++)
			{
				float y0 = y << 1;
				float y1 = std::min(y0 + 1, (float)std::max((U32)1, height >> (l-1)) - 1);
				for(int x = 0; x < std::max((U32)1, mipmap_width); x++)
				{
					float x0 = x<<1;
					float x1 = std::min(x0 + 1, (float)std::max((U32)1, width>>(l-1)) - 1);
					
					box[0] = pre_level[(U32)(y0 * pre_width) + (U32)x0];
					box[1] = pre_level[(U32)(y0 * pre_width) + (U32)x1];
					box[2] = pre_level[(U32)(y1 * pre_width) + (U32)x0];
					box[3] = pre_level[(U32)(y1 * pre_width) + (U32)x1];

					cur_level[(y * cur_width) + x] =	((U32)(((box[0] >> 24) + (box[1] >> 24) + (box[2] >> 24) + (box[3] >> 24)) * 0.25) << 24) |
														((U32)(((box[0] >> 16 & 0xFF) + (box[1] >> 16 & 0xFF) + (box[2] >> 16 & 0xFF) + (box[3] >> 16 & 0xFF)) * 0.25) << 16) |
														((U32)(((box[0] >> 8 & 0xFF) + (box[1] >> 8 & 0xFF) + (box[2] >> 8 & 0xFF) + (box[3] >> 8 & 0xFF)) * 0.25) << 8) |
														(U32)(((box[0] & 0xFF) + (box[1] & 0xFF) + (box[2] & 0xFF) + (box[3] & 0xFF)) * 0.25);
				}
			}

			pre_width = cur_width;
		}


		return mm_chain;
	}

	void TextureDDSCompiler::destroy_mipmap_chain(mipmap_chain *mm_chain)	
	{
		for(U32 i = 0; i < mm_chain->length; i++)
		{
			delete[] mm_chain->chain[i].data;
		}

		// Cant do this.. why?
		//delete[] mm_chain->chain;

		delete mm_chain;
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
