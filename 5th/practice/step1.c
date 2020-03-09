#include <signal.h>
#include <time.h>

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIGTIMER (SIGRTMAX)
#define THREADNUM 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t invocator = PTHREAD_COND_INITIALIZER;

timer_t timerid;
int ticks = 0;

timer_t SetTimer(int signo, int sec, int mode);
void *timerThread(void *arg);
void *timerThread2(void *arg);
void SignalHandler(int signo, siginfo_t * info, void *context);

int main(){
  struct sigaction sigact;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = 0;
  sigact.sa_sigaction = SignalHandler;

  if(sigaction(SIGTIMER, &sigact, NULL) == -1){
    perror("sigaction failed"); return -1;
  }
  else{
    printf("now timer is set\n");
  }

  sigaction(SIGINT, &sigact, NULL);
  timerid = SetTimer(SIGTIMER, 1000, 1);

  pthread_t threads[THREADNUM];
  pthread_create(&threads[0], NULL, timerThread, NULL);
  for(int i=0; i<THREADNUM; i++) pthread_join(threads[i], NULL);

  while(1){};

  return 0;
}

void *timerThread(void *arg){
  while(1){
    if(ticks < 100) pthread_cond_wait(&invocator, &mutex);
    printf("TIMER 1 > kimochi\n");
    ticks = 0;
  }


}

timer_t SetTimer(int signo, int sec, int mode){
  struct sigevent sigev;
  struct itimerspec itval;
  struct itimerspec oival;

  sigev.sigev_notify = SIGEV_SIGNAL;
  sigev.sigev_signo = signo;
  sigev.sigev_value.sival_ptr = &timerid;

  if(timer_create(CLOCK_REALTIME, &sigev, &timerid) == 0){
    itval.it_value.tv_sec = sec / 1000;
    itval.it_value.tv_nsec = (long)(sec % 1000) * (1000000L);

    if(mode == 1){
      itval.it_interval.tv_sec = itval.it_value.tv_sec;
      itval.it_interval.tv_nsec = itval.it_value.tv_nsec;
    } else {
      itval.it_interval.tv_sec = 0;
      itval.it_interval.tv_nsec = 0;
    }

    if(timer_settime(timerid, 0, &itval, &oival) != 0){
      perror("time_settime error!");
    }
    else{
      perror("time_create error!");
      return (timer_t)-1;
    }
    return timerid;
  }
}

void SignalHandler(int signo, siginfo_t * info, void *context){
  if(signo == SIGTIMER){
    printf("SIGTIMER ON\n");
    ticks++;
    pthread_cond_signal (&invocator);  // wake up the thread
  }
  else if(signo == SIGINT){
    timer_delete(timerid);
    perror("Ctrl + C cached!\n");
    exit(1);
  }
}
