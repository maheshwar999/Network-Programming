.c.o:
	gcc -c $<

all: server client

server: server.o api.o api.h
	gcc -o server server.o api.o

client: client.o api.o api.h
	gcc -o client client.o api.o


clean:
	rm -vf *.o
	rm -vf server client
