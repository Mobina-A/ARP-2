#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <signal.h>
#include <time.h>
#include <unistd.h> 
#include <errno.h>
#include <stdbool.h> // Added to define bool type

#define NUMBER_OF_INACTIVE_PROCESS  4
#define INACTIVE_TIME_LIMIT   60
#define CHECK_TIME   30

void sig_killhandler(int signo);
int checkTime(int hStart, int mStart, int sStart, int durationSeconds); // Changed return type

void sig_killhandler(int signo) {
    if (signo == SIGINT) {
        printf("I Received SIGINT Signal!\n");
        kill(getpid(), SIGKILL);
    }
}

int main(int argc, char const *argv[]) {
    FILE *fp;
    char *token;
    char buffer[100];
    int processPID;
    int lastActivityHour;
    int lastActivityMin;
    int lastActivitySec;
    int processActivityStatus; // Changed to int
    int inactiveProcessCounter = 0;
    char processLogAddress[6][20] = {"./drone.txt", "./server.txt", "./window.txt", "./master.txt"};
    char processPIDList[6][20] = {"", "", "", ""};
  
    while (1) {
        sleep(CHECK_TIME);
        while (inactiveProcessCounter < NUMBER_OF_INACTIVE_PROCESS) {
            fp = fopen(processLogAddress[inactiveProcessCounter], "r");
            if (fp == NULL) {
                printf("Could not open the 'mxCommand' pipe; errno=%d\n", errno);
                exit(1); 
            } 

            fseek(fp, 0, SEEK_SET);
            fread(buffer, 50, 1, fp);
            fclose(fp);

            token = strtok(buffer, ",");
            strcpy(processPIDList[inactiveProcessCounter], token);
            processPID = atoi(token);

            token = strtok(NULL,",");
            lastActivityHour = atoi(token);

            token = strtok(NULL,",");
            lastActivityMin = atoi(token);

            token = strtok(NULL,",");
            lastActivitySec = atoi(token);
      
            printf("The process %d last activity time is(H,M,S):%d:%d:%d \n",processPID, lastActivityHour, lastActivityMin, lastActivitySec);

            printf("Check Activity Status in last 60 seconds...\n");
            processActivityStatus = checkTime(lastActivityHour, lastActivityMin, lastActivitySec, INACTIVE_TIME_LIMIT);

            if (processActivityStatus) {
                printf("The process %d was inactive for more than 60 seconds... :(\n\n", processPID);
                inactiveProcessCounter++;
                if (inactiveProcessCounter == NUMBER_OF_INACTIVE_PROCESS) {
                    printf("Killing all the process :|\n");
                    for (int i = 0; i < NUMBER_OF_INACTIVE_PROCESS; i++) {
                        printf("Killing process %s\n", processPIDList[i]);
                        kill(atoi(processPIDList[i]), SIGINT);
                    }
                }
            } else {
                printf("Good! The process %d is active :)\n\n", processPID);
                inactiveProcessCounter = 0;
                break;
            }
        }
    }
    return 0; 
}

int checkTime(int hStart, int mStart, int sStart, int durationSeconds) {
    time_t t = time(NULL);  
    struct tm tm = *localtime(&t);
    int status = (tm.tm_hour - hStart) * 3600 + (tm.tm_min - mStart) * 60 + (tm.tm_sec - sStart) > durationSeconds; // Changed type to int
    if (status) {
        printf("Time Over\n");
        return 1; // Return 1 for true
    }
    return 0; // Return 0 for false
}

