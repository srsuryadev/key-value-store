CC = g++
CFLAGS = -g -Wall
RM = rm -rf

default: kvstore

kvstore: kvstore.o value.o record.o wal.o main.o
	$(CC) -L  $(CFLAGS) $^ -o $@

kvstore.o: kvstore.cc log_table.h #ephemeral_table.h log_table.h
	$(CC) -L  $(CFLAGS) -c $^ 

%.o: %.cc
	$(CC) -L  $(CFLAGS) -c $<

clean:
	$(RM) kvstore *.o
