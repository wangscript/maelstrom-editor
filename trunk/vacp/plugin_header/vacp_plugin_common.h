#ifndef VACP_PLUGIN_COMMON_H
#define VACP_PLUGIN_COMMON_H

#include <map>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>

struct cmp_str
{
	bool operator()(const char *a, const char *b)
	{
		return strcmp(a, b) < 0;
	}
};

// Should this really be needed?
struct cmp_stdstr
{
	bool operator()(std::string *a, std::string *b)
	{
		std::cout << "comparing " << *a << " with " << *b << " results in " << (a->compare(*b)) << std::endl;
		return (a->compare(*b) == 0);
	}
};

class Content
{
private:
    std::map<char*, char*, cmp_str> attributes;
public:
	bool exists(char *key)
	{
		std::map<char *, char*, cmp_str>::iterator it = this->attributes.find(key);
		return (it != this->attributes.end());
	}

    int *get_int_value(char *key)
    {
        int *return_value = NULL;
        std::map<char*, char*, cmp_str>::iterator it = this->attributes.find(key);
        if(it != this->attributes.end())
			return_value = reinterpret_cast<int*>(it->second);

		return return_value;
	}

	char *get_pchar_value(char *key)
	{
		char *return_value = NULL;
		std::map<char*, char*, cmp_str>::iterator it = this->attributes.find(key);
		if(it != this->attributes.end())
			return_value = it->second;

		return return_value;
	}

	bool *get_bool_value(char *key)
	{
		bool *return_value;
		std::map<char*, char*, cmp_str>::iterator it = this->attributes.find(key);
		if(it != this->attributes.end())
			return_value = reinterpret_cast<bool*>(it->second);

		return return_value;
	}

	int set_pchar_value(char *key, char *value)
	{
		std::map<char*, char*, cmp_str>::iterator it = this->attributes.find(key);
		if(it != this->attributes.end())
			return -1;

		this->attributes.insert(std::pair<char*, char*>(key, value));
		return 0;
	}
	
	int set_int_value(char *key, int *value)
	{
		return set_pchar_value(key, reinterpret_cast<char*>(value));
	}

	int set_bool_value(char *key, bool *value)
	{
		return set_pchar_value(key, reinterpret_cast<char*>(value));
	}

};

class ITextureData
{
public:
    virtual char *get_data(void) = 0;
    virtual unsigned int get_data_length(void) = 0;
    virtual int get_width(void) = 0;
    virtual int get_height(void) = 0;
    virtual unsigned int get_stride(void) = 0;
};

class InvalidConfigurationException : std::exception
{
private:
	std::string &msg;
public:
	explicit InvalidConfigurationException(std::string &message) : msg(message)
	{
	}
	virtual const char *what(void)
	{
		return this->msg.c_str();
	}
};

class FailedConversionException : std::exception
{
private:
	std::string &msg;
public:
	explicit FailedConversionException(std::string &message) : msg(message)
	{
	}
	virtual const char *what(void)
	{
		return this->msg.c_str();
	}
};

class ConfigMap
{
private:
	std::map<std::string*, std::string*, cmp_stdstr> conf_map;
public:
	explicit ConfigMap(char *config)
	{
		std::string config_string(config);
		std::string key;
		std::stringstream stream(config_string);
		while(std::getline(stream, key, ';'))
		{
			std::cout << "Found configkey '" << key << "'" << std::endl;
			std::string value;
			if(std::getline(stream, value, ';'))
			{
				std::cout << "Found configvalue '" << value << "'" << std::endl;
				std::string *pKey = new std::string(key);
				std::string *pValue = new std::string(value);
				this->conf_map.insert(std::pair<std::string*, std::string*>(pKey, pValue));
			}
		}
	}

	std::string *get_config(std::string &key)
	{
		std::string *value = NULL;
		std::cout << "get_config: " << key << std::endl;
		std::map<std::string*, std::string*, cmp_stdstr>::iterator it = this->conf_map.find(&key);
		if(it != this->conf_map.end())
		{
			std::cout << "Found " << key << ": " << (*it).first << "/" << (*it).second << std::endl;
			value = (*it).second;
		}
		return value;
		return NULL;	
	}
};

class ContentPlugin
{
private:
	ConfigMap *config;
	std::string last_error_msg;
protected:
	ContentPlugin()
	{
		this->config = NULL;
	}

	void parse_config(char *config_input)
	{
		if(this->config)
			delete this->config;
		this->config = new ConfigMap(config_input);
	}

	std::string *get_config(std::string &key)
	{
		return this->config->get_config(key);
	}

	int str_to_int(std::string &str)
	{
		int value;
		std::stringstream stream(str);
		stream >> value;
		// Stream.good() seems to return false even if conversion didnt fail. Must find another way to convert str to int.
		/*if(!stream.good())
		{
			std::string msg("Failed to convert string to int: ");
			msg.append(str);
			throw new FailedConversionException(msg);
		}*/
		return value;
	}

	void set_last_error_msg(std::string &msg)
	{
		this->last_error_msg = msg;
	}
public:
	std::string get_last_error_msg()
	{
		return this->last_error_msg;
	}
};

//template <class T>
class ContentExporter : public ContentPlugin
{
public:
    virtual Content* process(char *path) = 0;
    virtual void destroy(Content*) = 0;
};

class ContentFactoryBase
{
private:
	char *module_name;
	const char *config_info;
public:
	explicit ContentFactoryBase(char *name, const char *config) : module_name(name), config_info(config)
	{
	}

	char *get_name(void)
	{
		return this->module_name;
	}

	const char *get_config_info(void)
	{
		return this->config_info;
	}
};

//template <class T>
class ContentExporterFactory : public ContentFactoryBase
{
public:	
    explicit ContentExporterFactory(char *name, char *config) : ContentFactoryBase(name, config)
    {
    }

    virtual ContentExporter* create(void) = 0;
    virtual void destroy(ContentExporter*) = 0;
};

//template <class T>
class ContentCompiler : public ContentPlugin
{
public:
    virtual int process(Content *input, const char *path, char *config) = 0;
};

//template <class T>
class ContentCompilerFactory : public ContentFactoryBase
{
public:
    explicit ContentCompilerFactory(char *name, const char *config) : ContentFactoryBase(name, config)
    {
    }

    virtual ContentCompiler* create(void) = 0;
    virtual void destroy(ContentCompiler*) = 0;
};

#endif // VACP_PLUGIN_COMMON_H
