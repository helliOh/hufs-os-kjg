#include <signal.h>
#include <time.h>

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIGTIMER (SIGRTMAX)
#define THREADNUM 4

struct MULTARG{
  int id;
  int ticks;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conds[THREADNUM];

timer_t timerid;
int ticks[THREADNUM];

timer_t SetTimer(int signo, int sec, int mode);
void *timerThread(void *arg);
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

  for(int i=0; i<THREADNUM; i++){
    pthread_cond_init(&conds[i], NULL);

    int id = i;
    int tick;
    switch(id){
      case 0: tick = 1; break;
      case 1: tick = 3; break;
      case 2: tick = 7; break;
      case 3: tick = 11; break;
      default: break;
    }

    struct MULTARG *arg;
    arg = (struct MULTARG *)malloc(sizeof(struct MULTARG));
    arg->id = id;
    arg->ticks = tick;
    ticks[i] = 0;

    // printf("%d thread -> %d tick\n", arg->id, arg->ticks);
    pthread_create(&threads[i], NULL, timerThread, (void *)arg);
  }

  for(int i=0; i<THREADNUM; i++) pthread_join(threads[i], NULL);


  while(1);

  return 0;
}

void *timerThread(void *argv){
  struct MULTARG *arg = (struct MULTARG *)argv;
  int id = arg->id;
  int tick = arg->ticks;

  while(1){
    pthread_mutex_lock(&mutex);
    if(ticks[id] < tick) pthread_cond_wait(&conds[id], &mutex);
    else{
      time_t mytime = time(NULL);
      char * time_str = ctime(&mytime);
      time_str[strlen(time_str)-1] = '\0';
      for(int i=0; i<id; i++) printf("  ");
      printf("In thread %d :\n", id);
      for(int i=0; i<id; i++) printf("  ");
      printf("Current Time : %s\n", time_str);
      ticks[id] = 0;
    }
    pthread_mutex_unlock(&mutex);
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
    for(int i=0; i<THREADNUM; i++){
      ticks[i]++;
      pthread_cond_signal (&conds[i]);// wake up the thread
    }
    // for(int i=0; i<THREADNUM; i++) printf("thread %d : %d tick\n", i+1, ticks[i]);
  }
  else if(signo == SIGINT){
    timer_delete(timerid);
    perror("Ctrl + C cached!\n");
    exit(1);
  }
}
