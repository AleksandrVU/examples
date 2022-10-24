#include <iostream>
#include <DNSCache.h>
//#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <thread>
using namespace std;
typedef struct
{
    DNSCache *pDnsCache;
    volatile int num;
}pass_parameters_t;

#define N_THREADS               5000//251
#define TABLE_OUT_TIMEOUT_SEC   2
#define TIMES_TO_SHOW_DNS_TABLE 2

void outDnsTable(DNSCache *dnsCache)
{
    for (size_t i = 0; i < TIMES_TO_SHOW_DNS_TABLE; ++i)
    {
        std::cout << "===================================================================="<< std::endl;
        dnsCache->showTable();
        std::cout << "===================================================================="<< std::endl;
        sleep(TABLE_OUT_TIMEOUT_SEC);
    }

}

void updateDNS(DNSCache *dnsCache, int num)
{
    string numS = to_string(num&255);
    string name = "name_" + numS;//to_string(num);
    string ip = "192.168.1." + numS;
   // std::cout << name << " - " << ip << std::endl;
    dnsCache->update(name,ip);
    string getIp = dnsCache->resolve(name);
   // std::cout << "resolved "<< name << " - " << getIp << std::endl;

}


int main()
{
    DNSCache& dnsCache = DNSCache::getInstance();
    std::thread threads[N_THREADS];
    size_t i;
     for (i = 0; i < N_THREADS; ++i)
    {
        if (i)
        {
            threads[i] = std::thread(updateDNS, &dnsCache, i);
            continue;
        }
        threads[i] = std::thread( outDnsTable, &dnsCache);
    }

    for (i = 0; i < N_THREADS; ++i)
    {
        threads[i].join();
    }
    return 0;
}
