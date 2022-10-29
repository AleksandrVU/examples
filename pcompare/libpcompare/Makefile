VERSION       = 1.0.0
CC            = gcc
CXX           = g++
CFLAGS        = -Wall -g -O -fPIC
INCPATH       = -I../include 
COPY          = cp -fv
COPY_FILE     = cp -fv
COPY_DIR      = cp -f -R
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
MK_DIR        = mkdir -p
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
LINK          = g++
LDFLAGS       = -shared 
LIBS          = 
####### Output directory

OBJECTS_DIR   = ./
DISTLIB       = ../libs
DISTHDR       = ../include

####### Files

HEADER        = pcompare.h
SOURCES       = pcompare.c
OBJECTS       = pcompare.o
NAME          = libpcompare.so
TARGET        = $(NAME).$(VERSION)

first: all
####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LDFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

all: Makefile $(TARGET)

clean: 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) $(TARGET)
	

distclean: clean 
	-$(DEL_FILE) $(TARGET)
	-$(DEL_FILE) $(DISTLIB)/$(TARGET) 
	$(DEL_FILE) $(DISTLIB)/$(NAME) 

dist: $(TARGET)
	$(MK_DIR) $(DISTHDR)
	$(MK_DIR) $(DISTLIB)	
	$(COPY) $(TARGET) $(DISTLIB)
	$(COPY) $(HEADER) $(DISTHDR)
	$(SYMLINK) $(DISTLIB)/$(TARGET) $(DISTLIB)/$(NAME)
####### Compile
pcompare.o: pcompare.c 
	$(CC) -c $(CFLAGS) $(INCPATH) -o pcompare.o pcompare.c

