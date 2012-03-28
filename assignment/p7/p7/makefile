%.o: %.c
	gcc -c -DRPC_SVC_FG $<

all: simp.h client server

client: simpclient.o simp_clnt.o simp_xdr.o 
	gcc -o client simpclient.o simp_clnt.o simp_xdr.o 


server: simpservice.o simp_svc.o simp_xdr.o
	gcc -o server simpservice.o simp_svc.o simp_xdr.o


simp.h: simp.x
	rpcgen -C simp.x
	make
	exit

clean: 
	rm -vf *.o simp.h simp_svc.* simp_clnt.* simp_xdr.*
	rm -vf server client

