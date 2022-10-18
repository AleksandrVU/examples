CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -O2 -Wall -Wextra -fPIC $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -Wextra -fPIC $(DEFINES)
INCPATH       = -I. 
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
LIBS          = $(SUBLIBS) -ljson-c -lpthread -lcurl   
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.c 
OBJECTS       = main.o
DIST          = main.c
DESTDIR       = 
TARGET        = pcompare


first: all
####### Build rules

test_task:  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)


all: Makefile test_task

clean: 
	-$(DEL_FILE) $(OBJECTS)
	 $(DEL_FILE) $(TARGET)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 


####### Compile

main.o: main.c 
	$(CC) -c $(CFLAGS) $(INCPATH) -o main.o main.c

