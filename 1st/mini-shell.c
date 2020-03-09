#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned char BUFFSIZE = 0xff;

int main () {

   char buf[BUFFSIZE];//command buffer
   char usr[BUFFSIZE];//current username buffer
   char cwd[BUFFSIZE];//current working directory buffer

   char c;//buffer for a character
   char idx = 0;//command buffer index

   getlogin_r(usr, BUFFSIZE);//get current user
   getcwd(cwd, BUFFSIZE);//get current working directory

   printf("[%s@minishell] %s$", usr, cwd);//print the information of current user
   while(1){
     c=getchar();
     buf[idx++] = c;
     if(c ==  '\n'){
       printf("command accepted(type logout or ctrl+c to exit mini shell)\n");
       buf[--idx] = '\0';//since there will be carriage return in the last character in the buffer

       char *cmd = (char*) malloc(sizeof(char) * (idx+1));//create a string for satisfying parameter of system(const char)

       while(idx  > -1){
         cmd[idx] = buf[idx];
         idx--;
       }
       if(cmd != "logout"){
         system(cmd);
         free(cmd);
         printf("[%s@minishell] %s$", usr, cwd);
         idx = 0;
       }
       else exit(1);
     }
   }

   return(0);
}
