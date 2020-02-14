CC=gcc
CFLAGS=-I.

client: client.c helpers.c
		$(CC) -o client client.c  helpers.c -ljson-c -Wall
		
run: client
		./client

clean:
		rm -f *.o client
