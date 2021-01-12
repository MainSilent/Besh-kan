#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h> 
#include <stdbool.h>

struct User
{
    int num;
    char rid[16];
    char username[128];
    char lock[32];
    char admin[16];
};

void red();
void green();
void blue();
void reset();
void check_os(char *);
void get_users(char [], struct User *, int *);
void reboot();

int main() {
    int count = 0;
    char device[6];
    struct User Users[99];

    // run init script
    system("busybox chmod +x /init.sh && /init.sh");
    system("clear");

    // Besh-kan init
    printf("Welcome to Besh kan\n");

    // Check if any os have been found
    check_os(device);

    if (strlen(device) == 0) {
        red();
        printf("Error: can't find any operating system.\n");
        reset();
        reboot();
    }
    else {
	    blue();
		printf("Note: press enter to choose the default values.\n");
		reset();

        green();
        printf("Operating System Detected!\n");
        reset();
    }

    // Get Users
    get_users(device, Users, &count);
    // Show Users
    for (int i = 1; i <= count; i++) {
        printf(" %d  ->  %-32s %-6s\n",
            Users[i].num,
            Users[i].username,
            Users[i].admin[0] == 'A' ? "Yes" : " -");
    }
    printf("\n");

    system("sh");
        
    // avoid the program gets close
    while (1)
        continue;
    return 0;
}

void red() {
  printf("\033[31m");
}

void green() {
  printf("\033[32m");
}

void blue() {
	printf("\033[36m");
}

void reset() {
  printf("\033[0m");
}

// check for operating systems
void check_os(char *device) {
    printf("Searching for os:\n");

    struct dirent *de; 
    DIR *dr = opendir("/mnt"); 
    // search all mounted devices
    while ((de = readdir(dr)) != NULL) {
        if (de->d_name[0] == 's') {
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

// check for users
void get_users(char device[], struct User *Users, int *count) {
    char stdout[1035];
    char command[128];
    FILE *fp;

    printf("Searching for Users...\n");
    sprintf(command, "chntpw -l /mnt/%s/Windows/System32/config/SAM", device);

    // get users from sam file
    printf("Users:\n");
    printf("\n Number -----------Username-----------  Admin?\n");
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run chntpw\n");
        reboot();
    }

    while (fgets(stdout, sizeof(stdout), fp) != NULL) {
        // increase the num
        Users[*count].num = ++(*count);

        // RID
        char *data = strtok(stdout, "| ");
        strcpy(Users[*count].rid, data);

        // Username
        data = strtok(NULL, "| ");
        strcpy(Users[*count].username, data);

        // Admin?
        data = strtok(NULL, "| ");
        strcpy(Users[*count].admin, data);

        // Lock?
        data = strtok(NULL, "| ");
        strcpy(Users[*count].lock, data);
    }
    pclose(fp);


}

void reboot() {
    printf("Press enter to reboot: ");
    getchar();
    system("reboot -f");
}