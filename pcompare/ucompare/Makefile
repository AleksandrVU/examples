CC            = gcc
CXX           = g++
CFLAGS        = -pipe -O2 -Wall -Wextra -fPIC 
CXXFLAGS      = -pipe -O2 -Wall -Wextra -fPIC 
INCPATH       = -I../include 
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = -L../libs -lpcompare -ljson-c -lcurl -lrpmvercmp
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ucompare.c
OBJECTS       = ucompare.o
DESTDIR       = 
TARGET        = ucompare


first: all
####### Build rules

ucompare:  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)


all: Makefile ucompare

clean: 
	-$(DEL_FILE) $(OBJECTS)
	 $(DEL_FILE) $(TARGET)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 


####### Compile

ucompare.o: ucompare.c 
	$(CC) -c $(CFLAGS) $(INCPATH) -o ucompare.o ucompare.c

