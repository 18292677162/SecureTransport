# Makefile Module For Develop Team

.SUFFIXES:
.SUFFIXES:  .c .o

WORKDIR=.
INCLDIR=$(WORKDIR)/incl
LIBDIR=$(HOME)/lib
BINDIR=$(HOME)/bin

CC=gcc

INCLFLG= -I$(INCLDIR)
LIBFLG = -L$(LIBDIR)
CFLAG= -c -g $(INCLFLG) 
LIBS = 

VPATH = $(WORKDIR)/src

OBJ1 = appcryptapi.o  des.o  myipc_shm.o


libappinterface.so: $(OBJ1) 
	$(CC) -shared -fPIC $^  -o $@ 
	@cp $@ $(LIBDIR)

.c.o:
	$(CC) -shared -fPIC $(CFLAG) $< -D_ITCAST_OS_LINUX

#gcc 	  -shared -fPIC -c -g -I./incl   *.c--->*.o
	
.PHONY : clean
clean :
	rm  -f *.o
	rm  -f *.s
