#include <hlsl_compiler.h>
#include <sstream>

#define DX_ENV "DXSDK_DIR"

#define COMPILE_TIMEOUT		60000 // 1 minute should be ALOT more than enough to compile a single shader.
#define READ_BUFF_SIZE	128

#if defined(WIN32)
#define EXPORT	__declspec(dllexport)
#endif

#if defined(__linux)
#define EXPORT
#endif

std::string get_formatted_error(DWORD error)
{
	WCHAR *buffer;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&buffer),
		0,
		NULL);

	std::stringstream str;

	return str.str();
}


extern "C"
{
	HLSLCompiler::HLSLCompiler()
	{
		this->fxc_stderr_rd = NULL;
		this->fxc_stderr_wr = NULL;
	}

	int HLSLCompiler::process(IContent *input, const char *path, char *config)
	{
		ContentCompiler::parse_config(config);

		std::string fxc_path;
		if(!this->get_dxfxc_path(fxc_path))
		{
			return -1;
		}

		std::string fxc_args;
		if(!this->create_fxc_args(input->get_pchar_value("PATH"), path, fxc_args))
		{
			return -1;
		}
		
		if(!this->init_stderr_pipe())
		{
			return -1;
		}

		STARTUPINFO startup_info;
		ZeroMemory(&startup_info, sizeof(STARTUPINFO));
		startup_info.cb = sizeof(STARTUPINFO);
		startup_info.dwFlags |= STARTF_USESTDHANDLES;
		startup_info.hStdError = GetStdHandle(STD_ERROR_HANDLE);

		PROCESS_INFORMATION proc_info;
		ZeroMemory(&proc_info, sizeof(PROCESS_INFORMATION));

		// Most Win32 functions require wide strings.
		wchar_t *w_fxc_path;
		wchar_t *w_fxc_args;

		this->create_wide_strings(fxc_path.c_str(), &w_fxc_path);
		this->create_wide_strings(fxc_args.c_str(), &w_fxc_args);

		if(!CreateProcess(
			w_fxc_path,
			w_fxc_args,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&startup_info,
			&proc_info))
		{
			CloseHandle(this->fxc_stderr_rd);
			CloseHandle(this->fxc_stderr_wr);
			
			std::stringstream msg_stream("CreateProcess failed. Return value: ");
			DWORD errorcode = GetLastError();
			msg_stream << errorcode;
			ContentCompiler::set_last_error_msg(msg_stream.str());
			return -1;
		}

		WaitForSingleObject(proc_info.hThread, COMPILE_TIMEOUT);

		DWORD exit_code;
		GetExitCodeProcess(proc_info.hProcess, &exit_code);

		CloseHandle(proc_info.hProcess);
		CloseHandle(proc_info.hThread);
		CloseHandle(this->fxc_stderr_wr);

		if(exit_code != 0)
		{
			std::string error_info = this->get_pipe_data();
			ContentCompiler::set_last_error_msg(error_info);
			return -1;
		}

		return 0;
	}

	bool HLSLCompiler::create_wide_strings(const char *in, wchar_t **out)
	{
		int result = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, in, -1,  NULL, 0);

		if(result)
		{
			*out = new wchar_t[result];
			result = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS | MB_PRECOMPOSED, in, -1, *out, result);
		}

		if(!result)
		{
			if(*out)
				delete[] *out;

			std::stringstream message("MultiByteToWideChar failed with: ");
			message << GetLastError();
			ContentPlugin::set_last_error_msg(message.str());
		}

		return (result > 0);
	}

	bool HLSLCompiler::init_stderr_pipe()
	{
		bool success = true;
		SECURITY_ATTRIBUTES sec_attr;
		sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
		sec_attr.bInheritHandle = TRUE;
		sec_attr.lpSecurityDescriptor = NULL;
		
		if(!CreatePipe(&this->fxc_stderr_rd, &this->fxc_stderr_wr, &sec_attr, 0) ||
			!SetHandleInformation(this->fxc_stderr_rd, HANDLE_FLAG_INHERIT, 0))
		{
			this->close_stderr_pipe();
			success = false;
		}

		return success;
	}

	bool HLSLCompiler::get_dxfxc_path(std::string &out)
	{
		bool success;
		char *dxsdk_dir = getenv(DX_ENV);
		if(dxsdk_dir == NULL)
		{
			std::string msg("Can not find DXSDK_DIR environment variable. Have you installed the DX SDK?");
			ContentCompiler::set_last_error_msg(msg);
			success = false;
		}
		else
		{
			out.clear();
			//out.append("\""); // Quoted strings might not be needed when the path and arguments are passed as two separate strings.
			out.append(dxsdk_dir);
			out.append("Utilities\\bin\\");
#if defined(_WIN64)
			out.append("x64\\fxc.exe");
#else
			out.append("x86\\fxc.exe");
#endif
			success = true;
		}

		return success;
	}
	
	bool HLSLCompiler::create_fxc_args(const char *input_path, const char *output_path, std::string &out)
	{
		bool success;
		wchar_t *wide_input_path;
		wchar_t *wide_output_path;

		std::string debug_flag("Debug");
		std::string *debug_config = ContentCompiler::get_config(debug_flag);

		// Turn on debug mode if debug config is present and equal to "1".
		if(debug_config && debug_config->compare("1") == 0)
			out.append("fxc.exe /Od /Zi ");

		out.append("/T fx_4_0 ");

		out.append("/Fo ");
		out.append(output_path);
		out.append(" ");
		out.append(input_path);
		return success;
	}

	const std::string HLSLCompiler::get_pipe_data()
	{
		std::string error_info;
		char buffer[READ_BUFF_SIZE];
		DWORD bytes_read;

		while(ReadFile(this->fxc_stderr_rd, buffer, READ_BUFF_SIZE, &bytes_read, NULL) && bytes_read > 0)
		{
			error_info.append(buffer, bytes_read);
		}

		return error_info;
	}

	void HLSLCompiler::close_stderr_pipe()
	{
		if(this->fxc_stderr_rd)
		{
			CloseHandle(this->fxc_stderr_rd);
			this->fxc_stderr_rd = NULL;
		}

		if(this->fxc_stderr_wr)
		{
			CloseHandle(this->fxc_stderr_wr);
			this->fxc_stderr_wr = NULL;
		}
	}

	class HLSLCompilerFactory : public ContentCompilerFactory
	{
	public:
		HLSLCompilerFactory() : ContentCompilerFactory(
			"HLSL Compiler",
			"Debug;int;VS;list;bleh...;PS;list;bleh...;GS;list;bleh...")
		{
		}

		HLSLCompiler *create()
		{
			return new HLSLCompiler;
		}

		void destroy(ContentCompiler *compiler)
		{
			delete compiler;
		}
	};

	EXPORT ContentCompilerFactory *create_compiler_factory()
	{
		return new HLSLCompilerFactory;
	}
}
