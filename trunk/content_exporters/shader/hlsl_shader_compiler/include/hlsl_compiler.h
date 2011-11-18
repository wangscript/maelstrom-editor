#include <vacp_plugin_common.h>
#include <Windows.h>

enum VS_PROFILE
{
	vs_1_1,
	vs_2_0,
	vs_2_a,
	vs_2_sw,
	vs_3_0,
	vs_3_sw,
	vs_4_0,
	vs_4_1
};

enum PS_PROFILE
{
	ps_2_0,
	ps_2_a,
	ps_2_b,
	ps_2_sw,
	ps_3_0,
	ps_3_sw,
	ps_4_0,
	ps_4_1
};

enum GS_PROFILE
{
	gs_4_0,
	gs_4_1
};

class HLSLCompiler : public ContentCompiler
{
private:
	HANDLE fxc_stderr_rd;
	HANDLE fxc_stderr_wr;
	void close_stderr_pipe();
	bool init_stderr_pipe();
	bool HLSLCompiler::get_dxfxc_path(std::string &out);
	const std::string get_pipe_data();
	bool HLSLCompiler::create_fxc_args(const char *input_path, const char *output_path, std::string &out);
	bool create_wide_strings(const char *in, wchar_t **out);

public:
	HLSLCompiler();
	virtual int process(IContent *input, const char *path, char *config);
	
};