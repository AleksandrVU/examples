CC            = gcc
CXX           = g++
CFLAGS        = -pipe -g -Wall -Wextra -fPIC 
CXXFLAGS      = -pipe -g -std=gnu++14 -Wall -Wextra -fPIC 
INCPATH       = -I. 
DISTDIR       = ./
DEL_FILE      = rm -f
LINK          = g++
LFLAGS        = 
LIBS          = -lpthread    

SOURCES       = DNSCache.cpp \
		test_dns-cache.cpp 
OBJECTS       = DNSCache.o \
		test_dns-cache.o
DIST          = DNSCache.h \
		IDNSCache.h DNSCache.cpp \
		test_dns-cache.cpp
DESTDIR       = 
TARGET        = dns-cache


first: all
####### Build rules

dns-cache:  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)


all: Makefile dns-cache

clean: 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) $(TARGET) 

####### Compile

DNSCache.o: DNSCache.cpp DNSCache.h \
		IDNSCache.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DNSCache.o DNSCache.cpp

test_dns-cache.o: test_dns-cache.cpp DNSCache.h \
		IDNSCache.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o test_dns-cache.o test_dns-cache.cpp

