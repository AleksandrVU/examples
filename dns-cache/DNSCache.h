#ifndef DNSCACHE_H
#define DNSCACHE_H
#include "IDNSCache.h"
#include <map>
#include <shared_mutex>
#include <queue>

class DNSCache : public IDNSCache
{
private:
    DNSCache() = default;
    ~DNSCache() = default;
    DNSCache(const DNSCache &) = delete;
    const DNSCache& operator=(const DNSCache& i) = delete;

    void* operator new(std::size_t) = delete;
    void* operator new[](std::size_t) = delete;

    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;

    mutable std::shared_timed_mutex mtx;
    //std::mutex rmtx;
    std::map<std::string,std::string> dnsCache;
    std::queue<std::string> dnsQueue;

    void addToCache(const std::string& name, const std::string& ip);
    void deleteOldestItem();

public:
    static DNSCache& getInstance()
    {
        static DNSCache obj;
        return obj;
    }

    virtual void update(const std::string& name, const std::string& ip) override;
    virtual std::string resolve(const std::string& name) const override;

    void showTable();
};

#endif // DNSCACHE_H
