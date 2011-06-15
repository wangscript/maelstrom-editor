#ifndef VACP_PLUGIN_COMMON_H
#define VACP_PLUGIN_COMMON_H

class ITextureData
{
public:
    virtual unsigned char *get_data(void) = 0;
    virtual unsigned int get_data_length(void) = 0;
    virtual unsigned int get_stride(void) = 0;
};

template <class T>
class ContentExporter
{
public:
    virtual T* process(char *path) = 0;
    virtual void destroy(T*) = 0;
};

#if !defined(NO_PLUGIN_HEADER)
extern "C" __declspec(dllexport) void get_exporter_name(char *buffer, unsigned int buffer_size)
{
	strcpy_s(buffer, buffer_size, EXPORTER_NAME);
}
#endif


#endif // VACP_PLUGIN_COMMON_H
