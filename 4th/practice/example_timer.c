#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define TIMEOUT 	5	// login time limit = 5 sec. After alarm, ring a bell.
#define MAXTRIES 	5	// retry login five times when timeout
#define LINESIZE 	100	// login name/passwd buffer size
#define CTRL_G		'\007'	// bell
#define TRUE 		1
#define FALSE 		0

volatile int timed_out;	// set when an alarm occurs
char myline [LINESIZE];	// character buffer
void sig_catch(int);		// alarm signal handler

char *quickreply(char *prompt) {

	void (*was)(int);
	int ntries, i;
	char *answer;

	was = signal (SIGALRM, sig_catch);
	siginterrupt (SIGALRM, 1);
      // set error return when a signal occurs

	for (ntries = 0; ntries <MAXTRIES; ntries++) {  // retry loop

 		timed_out = FALSE;

 		printf("\n%s > ",prompt);
 		fflush(stdout);
 		alarm(TIMEOUT);

   	for (i = 0; i < LINESIZE; i++) {
 			if ((myline[i] = getchar()) < 0) break;  // error return by alarm
 			if (myline[i] == '\n') {
 				myline[i] = 0;
 		    break;  // end of line input
 			}
 		}
    
 		alarm(0);
 		if (!timed_out) break;
	}	// end of retry loop
       // normal or fail 5 times
  	answer = myline;
  	signal(SIGALRM,was);
		return(ntries == MAXTRIES ? ((char *) 0) : answer);
}

void sig_catch (int sig_no){
	timed_out = TRUE;
	putchar (CTRL_G);	// ring a bell
	fflush (stdout);		// insure that the bell-ring
	signal (SIGALRM, sig_catch);  // reinstall the user defined signal handler
}

int main(){
  quickreply("login-name:");
}
