#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
   (void)argc;
   (void)argv;
   int i;
   for (i = 0;; i++) {
       sleep(1);
       printf("%d\n", i);
   }
   return 0;
} 