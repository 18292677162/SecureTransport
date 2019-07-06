.PHONY:clean all

DIR=.
VPATH=./src

CC=gcc
CFLAGS=-Wall -g -I$(DIR)/inc/
LIBFLAG=-L$(DIR)/lib/

BIN=keymngclient keymngserver

all:$(BIN)

keymngclient:keymngclient.o keymngclientop.o keymnglog.o myipc_shm.o keymng_shmop.o
	$(CC) $(LIBFLAG) $^ -o $@ -lpthread -lsocket -lkeymng_msg

keymngserver:keymngserver.o keymngserverop.o keymnglog.o myipc_shm.o keymng_shmop.o keymng_dbop.o
	$(CC) $(LIBFLAG) $^ -o $@ -lpthread -lsocket -lkeymng_msg -lclntsh -licdbapi

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -rf *.o $(BIN)