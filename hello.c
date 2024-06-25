#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("MYVAL=%s\n", getenv("MYVAL"));
   // extern char **environ;
   // for(int i = 0; environ[i] != NULL; i++)
   //     printf("[%d]: %s\n", i, environ[i]);
    return 0;
}
