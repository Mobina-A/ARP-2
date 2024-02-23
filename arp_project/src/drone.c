#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <math.h>

// Drone working area
#define X_LIMIT_MIN    0
#define X_LIMIT_MAX    32
#define Y_LIMIT_MIN    0
#define Y_LIMIT_MAX    140

// Given parameters
double M = 1.0;            // Mass of the drone
double K = 0.1;            // Air friction coefficient
double deltaTime = 0.1;    // Time step 
double Fx = 0;             // Force in x direction
double Fy = 0;             // Force in y direction

// Initial conditions
double x = 1.0;   // Initial position in x direction
double y = 1.0;   // Initial position in y direction
double vx = 0.0;  // Initial velocity in x direction
double vy = 0.001;  // Initial velocity in y direction

// Function prototypes
void sig_killhandler(int signo);
void logData(char * fileName, char * mode);
void createLogFile(char * fileName, char * mode);
void moveDrone(char direction);
void calculateDroneMotion();

int main() {
    // Main logic goes here
    while (1) {
        // Get user input
        char ch;
        printf("Enter a character: ");
        scanf(" %c", &ch);

        // Process user input
        moveDrone(ch);
    }

    return 0;
}

void moveDrone(char direction) {
    // Compute the force in x and y directions
    switch (direction) {
        case 'e':
           Fx += -1;
           logData("drone.txt", "w+");
            break;
        case 'c':
           Fx += 1;
           logData("drone.txt", "w+");
            break;
        case 's':
            Fy += -1;
            logData("drone.txt", "w+");
            break;
        case 'f':
            Fy += 1;
            logData("drone.txt", "w+");
            break;
        case 'r':
            Fx += -sqrt(2)/2;
            Fy += sqrt(2)/2;
            logData("drone.txt", "w+");
            break;
        case 'x':
            Fx += sqrt(2)/2;
            Fy += -sqrt(2)/2;
            logData("drone.txt", "w+");
            break;
        case 'v':
            Fx += sqrt(2)/2;
            Fy += sqrt(2)/2;
            logData("drone.txt", "w+");
            break;
        case 'w':
            Fx += -sqrt(2)/2;
            Fy += -sqrt(2)/2;
            logData("drone.txt", "w+");
            break;
        case 'd':
            Fx=0;
            Fy=0;
            vx=0;
            vy=0;
            logData("drone.txt", "w+");
            break;
    }

    // Solve the dynamic equation by the Taylor series expansion
    calculateDroneMotion();
}

void calculateDroneMotion() {
    // Calculate velocities
    vx += (Fx - K * vx) / M * deltaTime;
    vy += (Fy - K * vy) / M * deltaTime;

    // Update positions
    x += vx * deltaTime + 0.5 * ((vx - K * vx) / M) * deltaTime * deltaTime;
    y += vy * deltaTime + 0.5 * ((vy - K * vy) / M) * deltaTime * deltaTime;

    // Check boundary conditions and adjust velocities
    if (x < X_LIMIT_MIN || x > X_LIMIT_MAX) {
        vx = -vx; // Reverse the velocity upon hitting X boundary
    }
    if (y < Y_LIMIT_MIN || y > Y_LIMIT_MAX) {
        vy = -vy; // Reverse the velocity upon hitting Y boundary
    }
}

void sig_killhandler(int signo){
    if (signo == SIGINT){
        printf("I Received SIGINT Signal!\n");
        exit(EXIT_SUCCESS);
    }
}

void createLogFile(char * fileName, char * mode){
    FILE *fp;
    if (remove(fileName) == 0) {
        printf("The old log file is deleted successfully!\n");
    }
    if ((fp = fopen(fileName, mode)) != NULL) {
        printf("The new log file is created successfully!\n");
        logData(fileName, mode);
    } else {
        printf("Could not create a log file!\n");
    }
}

void logData(char * fileName, char * mode){
   FILE *fp;
   int pid = getpid();
   time_t t = time(NULL);
   struct tm *tm_info = localtime(&t);
   
   fp = fopen(fileName, mode);
   if (fp != NULL){
      fprintf(fp, "%d,%d,%d,%d\n", pid, tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
      fclose(fp);
   } else {
      printf("Could not open the %s file; errno=%d\n", fileName, errno);
      exit(EXIT_FAILURE); 
    }
}

