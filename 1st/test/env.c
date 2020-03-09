#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
   char* envs = getenv("PATH");

   char *token = strtok(envs, ":");
   do {
      printf("%s\n", token);
   } while (token = strtok(NULL, ":"));

   return(0);
}
