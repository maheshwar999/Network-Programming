/*
 *  Created by Maheshwar on 03/11/11.
 *
 */
#include <stdio.h> 
#include <unistd.h> 
#include <string.h> 
#define MAXL 1000000
char buf[MAXL+10];
int main()
{
	// we want to execute ls -l | uniq || grep ^d, grep ^-
	int p1[2], p2[2];
	if (pipe(p2) < 0)
		perror("Pipe p2 error");
	int c1 = fork();
	if (c1 == 0) {
		// child 1, lets do uniq here
		if (pipe(p1) < 0)
			perror("Pipe p1 error");
		int c2 = fork();
		if (c2 == 0) {
			// lets do ls here
			dup2(p1[1], 1); close(p1[0]);
			execlp("ls", "ls", "-l", (char*) 0);
		} else {
			wait(NULL);
			
			dup2(p1[0], 0); close(p1[1]);
			dup2(p2[1], 1); close(p2[0]);
			execlp("uniq", "uniq", (char*) 0);
		}
	} else {
		// lets do greps here
		wait(NULL);
		close(p2[1]);
		int x;
		x = read(p2[0], buf, MAXL);
		buf[x] = 0;
		// fprintf(stderr, "%s\n", buf);
		// write it to two pipes
		int c = fork();
		if (c == 0) {
			int p31[2];
			if (pipe(p31) < 0)
			perror("Pipe 31 error");
			write(p31[1], buf, x); close(p31[1]);
			dup2(p31[0], 0);
			// write(1, "Directories\n", 12);
			execlp("grep", "grep", "^d", (char*) 0);
		} else {
			wait(NULL);
			int p32[2];
			if (pipe(p32) < 0)
			perror("Pipe 32 error");
			write(p32[1], buf, x); close(p32[1]);
			dup2(p32[0], 0);
			// write(1, "Files\n", 6);
			execlp("grep", "grep", "^-", (char*) 0);
		}
	}
	return 0;
}
