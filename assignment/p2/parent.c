/*
 *  Created by Maheshwar on 04/11/11.
 *
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h> 
#include <unistd.h> 
#define MAXN 101
struct my_msgbuf
{
	long mtype;
	char mtext[200];
};
#define SZTYPE 1
int pids[MAXN];
int main(int argc, char** argv) {
	int N,K;

	if (argc != 3) {
		printf("Usage: ./parent <N> <K>\n");
		exit(0);
	}
	sscanf(argv[1], "%d", &N);
	sscanf(argv[2], "%d", &K);

	printf("N:%d K:%d\n", N, K);
	// create N children
	// create message queue for IPC
	struct my_msgbuf buf;
	int msqid;
	if ((msqid = msgget (IPC_PRIVATE, IPC_CREAT | 0644)) == -1) {
		perror ("msgget");
		exit (1);
	}
	int i;
	for (i = 0; i < N; i++) {
		pids[i] = fork();
		if (pids[i] == 0) {
			// child i
			break;
		} else {
			fprintf(stderr, "Child %d: Pid %d\n", i, pids[i]);
		}
	}
	if (i < N) {
		// child i
		// read from message queue and exec
		if (msgrcv (msqid, &buf, sizeof (buf), getpid(), 0) == -1) {
			perror ("Msgrcv");
			exit (1);
		}
		fprintf(stderr, "Child %d Neighbour: %s\n", i, buf.mtext);
		char Msqid[10];
		sprintf(Msqid, "%d", msqid);
		execl("./game", "game", buf.mtext, Msqid, argv[2], (char*) 0);
		printf("No execlp\n");
		return 0;
	} else {
		// parent
		// write into message queue
		int j;
		for (j = 0; j < N; j++) {
			buf.mtype = pids[j];
			sprintf(buf.mtext, "%d", pids[(j+1)%N]);
			if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
				perror("Msgsnd");
				exit(1);
			}
		}
		buf.mtype = SZTYPE;
		sprintf(buf.mtext, "%d", N);
		if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
			perror("Msgsnd");
			exit(1);
		}
		sleep(2);
		// fprintf(stderr, "Start\n");
		if (kill(pids[0], SIGUSR1) == -1) {
			perror("Kill");
			exit(1);
		}

		for (j = 0; j < N; j++) wait(NULL);
	}
	return 0;
}

