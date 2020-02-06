CC = g++
CFLAGS = -g -Wall
RM = rm

default: kvstore

kvstore: kvstore.o value.o
	$(CC) $(CFLAGS) -o kvstore kvstore.o value.o

kvstore.o: kvstore.cc ephemeral_table.h log_table.h
	$(CC) $(CFLAGS) -c kvstore.cc

value.o: value.cc
	$(CC) $(CFLAGS) -c value.cc

clean:
	$(RM) kvstore *.o
