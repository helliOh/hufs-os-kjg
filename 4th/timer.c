#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define TIMEOUT 	5	// login time limit = 5 sec. After alarm, ring a bell.
#define MAXTRIES 	5	// retry login five times when timeout
#define LINESIZE 	100	// login name/passwd buffer size
#define CTRL_G		'\007'	// bell
#define TRUE 		1
#define FALSE 		0

volatile int timed_out;	// set when an alarm occurs
char buf_id [LINESIZE];	// character buffer
char buf_pw [LINESIZE];

char* password = "1234";

void sig_catch(int);		// alarm signal handler

char *input_PW(char *prompt) {

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
 			if ((buf_pw[i] = getchar()) < 0) break;  // error return by alarm
 			if (buf_pw[i] == '\n') {
 				buf_pw[i] = 0;
 		    break;  // end of line input
 			}
 		}

 		alarm(0);
 		if (!timed_out) break;
	}	// end of retry loop
       // normal or fail 5 times
  	answer = buf_pw;
  	signal(SIGALRM, was);
	return(ntries == MAXTRIES ? ((char *) 0) : answer);
}

char *input_ID(char *prompt) {

	void (*was)(int);
	int ntries, i;
	char *answer;

	was = signal (SIGALRM, sig_catch);
	siginterrupt (SIGALRM, 1); // set error return when a signal occurs

	for (ntries = 0; ntries <MAXTRIES; ntries++) {  // retry loop

 		timed_out = FALSE;

 		printf("\n%s > ",prompt);
 		fflush(stdout);
 		alarm(TIMEOUT);

   	for (i = 0; i < LINESIZE; i++) {
 			if ((buf_id[i] = getchar()) < 0) break;  // error return by alarm
 			if (buf_id[i] == '\n') {
        if(input_PW("login-password:") == 0){
          printf("\nlogin failed!\n");
          exit(0);
        }
        else{
           if(strcmp(buf_pw, password) == 0){
             printf("\nWelcome %s\n", buf_id);
             exit(0);
          }
           else{
             printf("\nlogin failure!\n");
             exit(0);
          }
        }
 				buf_id[i] = 0;
 		    break;  // end of line input
 			}
 		}

 		alarm(0);
 		if (!timed_out) break;
	}	// end of retry loop
       // normal or fail 5 times
  	answer = buf_id;
  	signal(SIGALRM, was);
	return(ntries == MAXTRIES ? ((char *) 0) : answer);
}

void sig_catch (int sig_no){
	timed_out = TRUE;
	putchar (CTRL_G);	// ring a bell
	fflush (stdout);		// insure that the bell-ring
	signal (SIGALRM, sig_catch);  // reinstall the user defined signal handler
}

int main(){
  if(input_ID("login-name:") == 0) printf("\nlogin failed!\n");
}
