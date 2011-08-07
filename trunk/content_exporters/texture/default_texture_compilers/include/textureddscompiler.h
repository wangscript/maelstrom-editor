#ifndef TEXTUREDDSCOMPILER_H
#define TEXTUREDDSCOMPILER_H

#include <../../../../vacp/plugin_header/vacp_plugin_common.h>
#include <ddsdef.h>

struct texture2d
{
	char *data;
	U32 length;
};

struct mipmap_chain
{
	texture2d *chain;
	U32 length;
};

class TextureDDSCompiler : public ContentCompiler
	{
	private:
		int prepare_dds_header(DDS_HEADER &header, IContent *input);
		void serialize_dds_header(DDS_HEADER& header, std::ofstream& output);
		bool is_power_of_two(U32 value);
		U32 get_set_bit(U32 value);
		mipmap_chain *generate_mipmap_chain(char *data, U32 data_length, U32 height, U32 width, U32 chain_length);
		void destroy_mipmap_chain(mipmap_chain *mm_chain);
	public:
		virtual int process(IContent *input, const char *path, char *config);
	};
#endif
