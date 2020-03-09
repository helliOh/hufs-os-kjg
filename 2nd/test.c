#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
  int fd[2];
  int input, output;
  int n; char buf[10];
  int status;
  pipe(fd);
  if(fork() == 0){
    input = open("input.txt", 0);
    close(fd[0]);
    while((n=read(input, buf, 10)) != 0)
      write(fd[1], buf, n);

      close(input); close(fd[1]); exit(0);
  }
  else{
    close(fd[1]);
    output = creat("output.txt", 0666);
    while((n=read(fd[0], buf, 10)) != 0)
      write(output, buf, n);

    close(output);  close(fd[0]);
    wait(&status);
  }
}
