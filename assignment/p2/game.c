/*
 *  Created by Maheshwar on 04/11/11.
 *
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <assert.h> 

#define SZTYPE 1
static int nxtpid, msqid, K;
struct my_msgbuf
{
	long mtype;
	char mtext[200];
};
struct my_msgbuf buf;
void myhandler_1(int signo) {
	// parent has called this child for first time
	// printf("%d SIGUSR1 Value of K %d Msqid %d\n", getpid(), K, msqid);
	buf.mtype = nxtpid;
	sprintf(buf.mtext, "%d", K);
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
		perror("Msgsnd");
		exit(1);
	}
	if (kill(nxtpid, SIGUSR2) == -1) {
		perror("Kill");
		exit(1);
	}
}
int passive;
int lastK = -1;

void myhandler_2(int signo) {
	// read a msg meant for you
	int k , sz = -1;
	// printf("%d SIGUSR2 Value of K %d Msqid %d\n", getpid(), K, msqid);
	if (msgrcv (msqid, &buf, sizeof (buf), getpid(), 0) == -1) {
		perror ("Msgrcv");
		exit (1);
	}
	// printf("%d Buf.mtext %s\n", getpid(), buf.mtext);
	k = atoi(buf.mtext);
	printf("process %d received %d\n", getpid(), k);

	if (msgrcv (msqid, &buf, sizeof (buf), SZTYPE, 0) == -1) {
		perror ("Msgrcv");
		exit (1);
	}
	sz = atoi(buf.mtext);
	// printf("%d Number of active players: %d\n", getpid(), sz);

	int kk = -1;
	if(passive) kk = k;
	else {
		if (sz == 1 && lastK == k && k == (K-1)) {
			printf("i am a truthful process %d, only the truth always wins\n", getpid());
			// clear msg queue
			if (msgctl (msqid, IPC_RMID, NULL) == -1) {
				perror ("msgctl");
				exit (1);
			}
			return;
		}
		if (k == 0) {
			sz -= 1;
			kk = K;
			passive = 1;
			printf("i am a foolish process %d, defeated\n", getpid());
		}
		else kk = k - 1;
	}
	lastK = kk;
	assert(kk >= 0 && sz > 0);
	// send out msg to neighbour
	// printf("%d Sending to %d val of k %d\n", getpid(), nxtpid, kk);
	buf.mtype = nxtpid;
	sprintf(buf.mtext, "%d", kk);
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
		perror("Msgsnd");
		exit(1);
	}
	buf.mtype = SZTYPE;
	sprintf(buf.mtext, "%d", sz);
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
		perror("Msgsnd");
		exit(1);
	}
	if (kill(nxtpid, SIGUSR2) == -1) {
		perror("Kill");
		exit(1);
	}

}
int main(int argc, char** argv) {
	signal(SIGUSR1, myhandler_1);
	signal(SIGUSR2, myhandler_2);
	if (argc != 4) {
		printf("Usage: ./game <Next Pid> <Msqid> <K>\n");
		exit(0);
	}
	sscanf(argv[1], "%d", &nxtpid);
	sscanf(argv[2], "%d", &msqid);
	sscanf(argv[3], "%d", &K);
	fprintf(stderr, "Pid:%d NxtPid:%d Msqid:%d K:%d\n", getpid(), nxtpid, msqid, K);
	int iters = 20;
	while(1) {
		// printf("Pid:%d Iters:%d\n", getpid(), iters);
		pause();
	}
	return 0;
}

