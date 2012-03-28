all : dieWithError.o fillAddrStruct.o LiPro.o Node.o
	gcc -o Server LiPro.o fillAddrStruct.o dieWithError.o
	gcc -o Client Node.o fillAddrStruct.o dieWithError.o

fillAddrStruct.o : fillAddrStruct.c
	gcc -c fillAddrStruct.c

dieWithError.o : dieWithError.c
	gcc -c dieWithError.c

LiPro.o : LiPro.c 
	gcc -c LiPro.c

Node.o : Node.c
	gcc -c Node.c
clean :
	rm -vf *.o
	rm -vf Server
	rm -vf Client
