#ifndef TEXTUREDDSCOMPILER_H
#define TEXTUREDDSCOMPILER_H

#include <../../../../vacp/plugin_header/vacp_plugin_common.h>
#include <ddsdef.h>

class TextureDDSCompiler : public ContentCompiler
	{
	private:
		int prepare_dds_header(DDS_HEADER &header, IContent *input);
		void serialize_dds_header(DDS_HEADER& header, std::ofstream& output);
	public:
		virtual int process(IContent *input, const char *path, char *config);
	};
#endif
