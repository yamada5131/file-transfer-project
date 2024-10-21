CC = gcc
CFLAGS = -Wall
LIBS = -lcrypt

all: server client

server: server.c user.o
	$(CC) $(CFLAGS) -o server server.c user.o $(LIBS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c

user.o: user.c
	$(CC) $(CFLAGS) -c user.c

clean:
	rm -f *.o server client
