#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <signal.h>
#include <time.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <errno.h>
#include "blackboard.h"

#define NUM_TARGETS 5

int pipe_fd[2]; // Pipe file descriptors

void initializeTargets(Point *targets_location);
void updateTargets(Point *targets_location, Blackboard *shared_memory);
void sig_killhandler(int signo);
void logData(char * fileName, char * mode);
void createLogFile(char * fileName, char * mode);

int main() {
    /*Creating a 'Log File' to record the process information like 'PID' and 'Last Activity Time'*/
    createLogFile("./targets.txt","w+");
    /*Logging the data*/
    logData("targets.txt","w+");
    //a 'SIGINT' signal from the 'watchdog process' to the 'motorx process'.
    signal(SIGINT, sig_killhandler);

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    while (1) {
        Blackboard shared_memory; // Create a local Blackboard struct
        initializeTargets(shared_memory.targets);
        updateTargets(shared_memory.targets, &shared_memory);

        // Write the data to the pipe
        if (write(pipe_fd[1], &shared_memory, sizeof(Blackboard)) == -1) {
            perror("Write to pipe failed");
            exit(EXIT_FAILURE);
        }

        sleep(1); // Adjust sleep duration for desired update rate (e.g., 1 second)
    }

    return 0;
}

void initializeTargets(Point *targets_location) {
    for (int i = 0; i < NUM_TARGETS; ++i) {
        targets_location[i].x = rand() % 30;
        targets_location[i].y = rand() % 150;
    }
}

void updateTargets(Point *targets_location, Blackboard *shared_memory) {
    for (int i = 0; i < NUM_TARGETS; ++i) {
        if (shared_memory->targets[i].x != -1 && shared_memory->targets[i].y != -1) {
            if (shared_memory->targets[i].x == shared_memory->drone.x &&
                shared_memory->targets[i].y == shared_memory->drone.y) {
                shared_memory->targets[i].x = -1;
                shared_memory->targets[i].y = -1;  // Mark as reached
            }
        }
    }
}

void sig_killhandler(int signo) {
    if (signo == SIGINT) {
        printf("I Received SIGINT Signal!\n");
        // Close the pipe
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        // Kill the process
        kill(getpid(), SIGKILL);
    }
}

void createLogFile(char * fileName, char * mode) {
    FILE *fp;
    if (!remove(fileName)) {
        printf("The old log file is deleted successfully!\n");
    }
    if (fp = fopen(fileName, mode)) {
        printf("The new log file is created successfully!\n");
        logData(fileName, mode);
    } else {
        printf("Could not create a log file!\n");
    }
}

void logData(char * fileName, char * mode) {
    FILE *fp;
    int pid = getpid();
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
   
    fp = fopen(fileName, mode);
    if (fp < 0) {
        printf("Could not open the %s file; errno=%d\n", fileName, errno);
        exit(1); 
    }
    fprintf(fp, "%d,%d,%d,%d\n", pid, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fclose(fp);
}
