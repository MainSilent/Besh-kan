#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    putenv("DPKG_MAINTSCRIPT_NAME=perl_5.32.0-5_amd64");
    putenv("DPKG_MAINTSCRIPT_PACKAGE=perl_5.32.0-5_amd64.deb");

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
//Reset the user password in any operating systems.
