#ifndef ERROR_H
#define ERROR_H

#include <QString>
#include <exception>
#include <string>

class NoSuchAssetException : public std::exception
{
    virtual const char *what() const throw()
    {
        return "No such asset was found.";
    }
};

class DirectoryAccessException : public std::exception
{
private:
	QString &path;
public:
	DirectoryAccessException(QString &path)
		: path(path)
	{
	}

	virtual const char *what() const throw()
	{
		QString message("Unable to access folder: ");
		message.append(path);
		return message.toLocal8Bit().constData();
	}
};

class FileNotFoundException : public std::exception
{
private:
    QString &path;
public:
    FileNotFoundException(QString &path)
        : path(path)
    {
    }

    virtual const char *what() const throw()
    {
        QString message("File was not found: ");
        message.append(path);
        return message.toLocal8Bit().constData();
    }
};

class DBException : public std::exception
{
private:
    QString &msg;
public:
    DBException(QString &message)
        : msg(message)
    {
    }

    virtual const char *what() const throw()
    {
        return msg.toLocal8Bit().constData();
    }
};


class InvalidAssetException : public std::exception
{
private:
    QString &msg;
public:
    InvalidAssetException(QString &message)
        : msg(message)
    {
    }

    virtual const char *what() const throw()
    {
        return msg.toLocal8Bit().constData();
    }
};

#endif // ERROR_H
