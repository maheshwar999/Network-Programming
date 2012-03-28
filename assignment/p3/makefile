all: mainserver childserver client

mainserver:	mainserver.c
	gcc -pthread mainserver.c -o mainserver
	
childserver: childserver.c
	gcc -pthread -g childserver.c -o childserver
	
client: client.c
	gcc -pthread client.c -o client
	
clean:	
	rm client mainserver childserver
