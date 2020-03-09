#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

unsigned char BUFFSIZE = 0xff;

int main (){
  char buf[BUFFSIZE];//command buffer
  char usr[BUFFSIZE];//current username buffer
  char cwd[BUFFSIZE];//current working directory buffer

  char c;//buffer for a character
  char idx = 0;//command buffer index

  getlogin_r(usr, BUFFSIZE);//get current user to execute user program
  getcwd(cwd, BUFFSIZE);//get current working directory

  printf("Welcome to small shell\n");
  printf("login with %s in %s\n", usr, cwd);
  printf("[%s@smallshell]$", usr);//print the information of current user

  while(1){
    c=getchar();
    buf[idx++] = c;

    if(c ==  '\n'){//get a line string
      printf("command accepted (type logout or ctrl+c to exit mini shell)\n");
      buf[--idx] = '\0';//since there will be carriage return in the last character in the buffer

      char *cmd = (char*) malloc(sizeof(char) * (idx+1));//create a string for satisfying parameter of system(const char)

      while(idx  > -1){
        cmd[idx] = buf[idx];
        idx--;
      }

      char* logout = strstr(cmd, "logout");

      if(logout == NULL){
        pid_t pid;
        int status;
        char* background = strchr(cmd, '&');

        if(background == NULL){//foreground
          if((pid = fork()) == 0){//child
            system(cmd);
          }
          else{//parent
            while (pid != wait(&status));
          }
        }
        else{//background
          if((pid = fork()) == 0){//child
            system(cmd);
          }
        }

      }
      else{
        printf("logout!!!\n");//logout
        exit(0);
      }

      free(cmd);
      printf("[%s@smallshell]$", usr);
      idx = 0;
    }

  }


}
