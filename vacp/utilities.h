#ifndef UTILITIES_H
#define UTILITIES_H

class Utilities
{
public:
    static quint32 calc_str_hash(const char *str)
    {
        quint32 hash = 5381;
        quint32 c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c;

        return hash;
    }
};

#endif // UTILITIES_H
