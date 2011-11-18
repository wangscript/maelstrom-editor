#ifndef ERROR_H
#define ERROR_H

#include <QString>
#include <exception>

class NoSuchContentException : public std::exception
{
private:
    QString name;
public:
    NoSuchContentException(QString content_name)
        : name(content_name)
    {
    }

    virtual const char *what() const throw()
    {
        QString message("Content with name '" + name + "' does not exist.");
        return message.toLocal8Bit().constData();
    }
};

class NoSuchPackageException : public std::exception
{
private:
    QString name;
public:
    NoSuchPackageException(QString package_name)
        : name(package_name)
    {
    }

    virtual const char *what() const throw()
    {
        QString message("Package with name '" + name + "' does not exist.");
        return message.toLocal8Bit().constData();
    }
};

class DirectoryAccessException : public std::exception
{
private:
        QString path;
public:
        DirectoryAccessException(QString path)
		: path(path)
	{
	}

	virtual const char *what() const throw()
	{
            QString message("Unable to access folder '" + path + "'");
            return message.toLocal8Bit().constData();
	}
};

class IOException : public std::exception
{
private:
    QString message;
public:
    IOException(QString message)
        : message(message)
    {
    }

    virtual const char *what() const throw()
    {
        return message.toLocal8Bit().constData();
    }
};

class FileNotFoundException : public std::exception
{
private:
    QString path;
public:
    FileNotFoundException(QString path)
        : path(path)
    {
    }

    virtual const char *what() const throw()
    {
        QString message("File was not found '" + path + "'");
        return message.toLocal8Bit().constData();
    }
};

class DBException : public std::exception
{
private:
    QString message;
public:
    DBException(QString failure_message)
        : message(failure_message)
    {
    }

    virtual const char *what() const throw()
    {
        return message.toLocal8Bit().constData();
    }
};


class InvalidContentException : public std::exception
{
private:
    QString name;
    QString message;
public:
    InvalidContentException(QString content_name, QString validation_message)
        : name(content_name),
          message(validation_message)
    {
    }

    virtual const char *what() const throw()
    {
        QString message("Asset '" + name + "' is invalid. Message: " + message);
        return message.toLocal8Bit().constData();
    }
};

class ExportFailureException : public std::exception
{
private:
        QString message;
public:
        ExportFailureException(QString failure_message)
                : message(failure_message)
        {
        }

        virtual const char *what() const throw()
        {
                return message.toLocal8Bit().constData();
        }
};

class CompileFailureException : public std::exception
{
private:
        QString message;
public:
        CompileFailureException(QString failure_message)
                : message(failure_message)
        {
        }

        virtual const char *what() const throw()
        {
                return message.toLocal8Bit().constData();
        }
};

#endif // ERROR_H
