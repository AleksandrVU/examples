#ifndef IDNSCACHE_H
#define IDNSCACHE_H
#include <string>
class IDNSCache
{
public:
    virtual void update(const std::string& name, const std::string& ip) = 0;
    virtual std::string resolve(const std::string& name) const = 0;
};

#endif // IDNSCACHE_H
