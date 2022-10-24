#include "DNSCache.h"
#include <iostream>

const unsigned long MAX_CACHE_SIZE = 8;

std::string DNSCache::resolve(const std::string& name) const
{
    const auto it = dnsCache.find(name);
    if (it == dnsCache.end()) return std::string("");
    return  it->second;
}

void DNSCache::deleteOldestItem()
{
    const std::string keyToDel = dnsQueue.front();
    dnsCache.erase(keyToDel);
    dnsQueue.pop();
}


void DNSCache::addToCache(const std::string &name, const std::string &ip)
{
    if (dnsCache.size() >= MAX_CACHE_SIZE)
    {
        deleteOldestItem();
    }
    dnsCache.emplace(name,ip);
    dnsQueue.emplace(name);
}

void DNSCache::update(const std::string &name, const std::string &ip)
{
    mtx.lock();
    const auto it = dnsCache.find(name);
    if (it == dnsCache.end()) addToCache(name, ip);
    else it->second = ip;
    mtx.unlock();
}

void DNSCache::showTable()
{
    size_t count =0;
    for (auto it : dnsCache)
    {
        std::cout << ++count << " : " << it.first << " - " << it.second << std::endl;
    }
}
