#ifndef VACP_PLUGIN_COMMON_H
#define VACP_PLUGIN_COMMON_H

#include <map>
#include <vector>
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

class IContent
{
public:
	virtual bool exists(char *) = 0;
	virtual int *get_int_value(char *) = 0;
	virtual char *get_pchar_value(char *) = 0;
	virtual bool *get_bool_value(char *) = 0;
};


class Content : public IContent
{
private:
	std::map<unsigned long, char*> *attributes;

	unsigned long calc_str_hash(char *str)
	{
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
	}
public:

	Content()
	{
		this->attributes = new std::map<unsigned long, char*>;
	}

	bool exists(char *key)
	{
		int hash = Content::calc_str_hash(key);
		std::map<unsigned long, char*>::iterator it = this->attributes->find(hash);
		return (it != this->attributes->end());
	}

	int *get_int_value(char *key)
	{
		int *return_value = NULL;
		int hash = Content::calc_str_hash(key);
		std::map<unsigned long, char*>::iterator it = this->attributes->find(hash);
		if(it != this->attributes->end())
			return_value = reinterpret_cast<int*>(it->second);

		return return_value;
	}

	char *get_pchar_value(char *key)
	{
		char *return_value = NULL;
		int hash = Content::calc_str_hash(key);
		std::map<unsigned long, char*>::iterator it = this->attributes->find(hash);
		if(it != this->attributes->end())
			return_value = it->second;
		int ss = this->attributes->size();
		return return_value;
	}

	bool *get_bool_value(char *key)
	{
		bool *return_value;
		int hash = Content::calc_str_hash(key);
		std::map<unsigned long, char*>::iterator it = this->attributes->find(hash);
		if(it != this->attributes->end())
			return_value = reinterpret_cast<bool*>(it->second);

		return return_value;
	}

	int set_pchar_value(char *key, char *value)
	{
		int hash = Content::calc_str_hash(key);
		std::map<unsigned long, char*>::iterator it = this->attributes->find(hash);
		if(it != this->attributes->end())
			return -1;

		this->attributes->insert(std::pair<int, char*>(hash, value));
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
	virtual IContent* process(char *path) = 0;
	virtual void destroy(IContent*) = 0;
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
	virtual int process(IContent *input, const char *path, char *config) = 0;
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
