#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    sleep(1);
    printf("\033c");
    printf("Welcome to Besh kan\n");

    system("chmod 0744 ./init.sh && ./init.sh");
    
    // avoid the program get close
    while (1)
        continue;
    return 0;
}