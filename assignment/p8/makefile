all: trace

trace: main.c recv_v4.o sig_alrm.o traceloop.o
	gcc -o trace main.c recv_v4.o sig_alrm.o traceloop.o
	
recv_v4.o: recv_v4.c
	gcc -c recv_v4.c
	
sig_alrm.o: sig_alrm.c
	gcc -c sig_alrm.c
	
traceloop.o: traceloop.c
	gcc -c traceloop.c
