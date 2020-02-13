CC = g++
CFLAGS = -g -Wall
RM = rm -rf

default: kvstore

kvstore: kvstore.o value.o record.o main.o
	$(CC) $(CFLAGS) $^ -o $@

kvstore.o: kvstore.cc #ephemeral_table.h log_table.h
	$(CC) $(CFLAGS) -c $^ 

%.o: %.cc
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) kvstore *.o
