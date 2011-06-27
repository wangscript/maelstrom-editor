#ifndef TEXTUREDDSCOMPILER_H
#define TEXTUREDDSCOMPILER_H

#include <../../../../vacp/plugin_header/vacp_plugin_common.h>
#include <ddsdef.h>

class TextureDDSCompiler : public ContentCompiler
	{
	private:
		int prepare_dds_header(DDS_HEADER &header, Content *input);
		void serialize_dds_header(DDS_HEADER& header, std::ofstream& output);
	public:
		virtual int process(Content *input, const char *path, char *config);
	};
#endif
