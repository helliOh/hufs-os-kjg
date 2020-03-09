#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

unsigned char BUFFSIZE = 10;

int main(){
  int fd[2];
  int n;
  char buf[BUFFSIZE];
  pipe(fd);

  pid_t pid;
  int status;
  pid = fork();

  if(pid > 0 ){//parent - do the print
    int output = creat("output.txt", 0666);

    close(fd[1]);

    while((n = read(fd[0], buf, BUFFSIZE)) != 0){
      write(output, buf, n);
    }

    close(output);
    close(fd[0]);
    waitpid(pid, &status, 0);

    printf("now child is dead\n");
    system("ps aux | grep pipe-aToA");
    printf("parent is dead\n");

    return 0;
  }
  else{//child fork
    printf("now child created. \n");
    close(fd[0]);//close unnecessary channel : stdout between child and parent

    int output = creat("output.txt", 0666);
    int child_fd[2];
    pid_t child_pid;
    int child_status;

    pipe(child_fd);
    child_pid = fork();

    if(child_pid > 0){//child main - modify the data

      int offset_ascii = 32;
      char small_letters[26] = "abcdefghijklmnopqrstuvwxyz";
      close(child_fd[1]);//stdout between child and grandchild

      while((n = read(child_fd[0], buf, BUFFSIZE)) != 0){

        for(int i=0; i<n; i++){
          for(int j=0; j<26; j++)
            if(buf[i] == small_letters[j]){
              buf[i] -= offset_ascii;
            }
        }

        write(fd[1], buf, n);
      }

      close(child_fd[0]);
      close(fd[1]);
      wait(&child_status);
      printf("now grandchild is dead\n");
      system("ps aux | grep pipe-aToA");
      exit(0);
    }
    else{//grandchild main - read from input
      printf("now grandchild created. \n");
      system("ps aux | grep pipe-aToA");

      int input = open("input.txt", 0);
      close(fd[1]);//stdout between child and parent
      close(child_fd[0]);//stdin between child and grandchild

      while((n = read(input, buf, BUFFSIZE)) != 0)
        write(child_fd[1], buf, n);

      close(input);
      close(child_fd[1]);
      exit(0);

    }
  }

  return 0;
}
