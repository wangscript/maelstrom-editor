#include <vacp_plugin_common.h>

#if defined(__linux)
#include <stdint.h>
typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int 		U32;
typedef signed int		I32;

#define PACKED(definition) \
	definition __attribute__((packed));
#endif
#if defined(WIN32)
typedef unsigned __int8		U8;
typedef unsigned __int16	U16;
typedef unsigned __int32	U32;
typedef __int32			I32;

#define PACKED(definition) \
	__pragma(pack(push, 1)) \
	definition; \
	__pragma(pack(pop))

#endif

class BasicShaderExporter : public ContentExporter
{
public:
	virtual IContent *process(char *path);
	virtual void destroy(IContent *data);
};