CC = g++
CFLAGS = -g -Wall -Wextra
RM = rm -rf

default: kvstore

kvstore: kvstore.o value.o record.o wal.o log_table.o main.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cc
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) kvstore *.o
