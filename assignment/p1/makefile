all: pserver tcpserver tcpclient udpclient udpserver

pserver: pserver.c 
	gcc -pthread pserver.c -o pserver

tcpclient:	tcpclient.c
	gcc tcpclient.c -o tcpclient
	
tcpserver:	tcpserver.c
	gcc tcpserver.c -o tcpserver
	
udpclient: udpclient.c
	gcc udpclient.c -o udpclient
	
udpserver: udpserver.c
	gcc udpserver.c -o udpserver
	
clean: 
	rm tcpclient tcpserver pserver udpserver udpclient
