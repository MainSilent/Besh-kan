#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h> 

void red();
void green();
void reset();
void check_os(char *);

int main() {
    char device[6];

    // run init script
    system("busybox chmod +x /init.sh && /init.sh");
    system("sleep 5 && clear");

    // Besh-kan init
    printf("Welcome to Besh kan\n");
    check_os(device);

    if (strlen(device) == 0) {
        red();
        printf("Error: can't find any operating system!\n");
        reset();
    }
    else {
        green();
        printf("os found\n");
        reset();
    }
    printf("%s\n", device);

    // avoid the program gets close
    while (1)
        continue;
    return 0;
}

void red() {
  printf("\033[0;31m");
}

void green() {
  printf("\033[0;32m");
}

void reset () {
  printf("\033[0m");
}

// check for operating systems
void check_os(char *device)
{
    printf("Searching for os:\n");

    struct dirent *de; 
    DIR *dr = opendir("/mnt"); 
    // search all mounted devices
    while ((de = readdir(dr)) != NULL) {
        if(de->d_name[0] == 's') {
            // check if the sam and system file exist
            char sam_path[sizeof(de->d_name)+50];
            char system_path[sizeof(de->d_name)+50];
            sprintf(sam_path, "/mnt/%s/Windows/System32/config/SAM", de->d_name);
            sprintf(system_path, "/mnt/%s/Windows/System32/config/SYSTEM", de->d_name);

            printf("Checking %s...\n", de->d_name);
            // set device variable to device name
            if (access(sam_path, R_OK|W_OK) == 0 && access(system_path, R_OK|W_OK) == 0) {
                for(int i=0; i < 7; ++i)
                    device[i] = de->d_name[i];
                break;
            }
        }
    }
    closedir(dr);
}