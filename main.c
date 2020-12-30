#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // run mount.sh
    system("chmod 0744 ./mount.sh && ./mount.sh && sleep 1 && clear");

    // init 
    printf("Welcome to Besh kan\n");
    system("sh");
    
    // avoid the program get close
    while (1)
        continue;
    return 0;
}