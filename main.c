#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // run init script
    system("busybox chmod +x /init.sh && /init.sh");
    system("clear");

    // Besh-kan init 
    printf("Welcome to Besh kan\n");
    system("sh");
    
    // avoid the program gets close
    while (1)
        continue;
    return 0;
}