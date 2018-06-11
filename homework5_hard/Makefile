all: client.o server.o

client.o: client.c
	gcc -o client client.c utils.h

server.o: server.c
	gcc -o server server.c utils.h

clean:
	rm -f *.o client server

pack: 
	zip hw5.zip Makefile server.c client.c utils.h