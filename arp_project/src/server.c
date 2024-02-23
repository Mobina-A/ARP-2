#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int *ptr;
#define SHM_SIZE 100
char *sharedMemoryName =  "blackboard";  

int sockfd, newsockfd, clilent;
struct sockaddr_in serv_addr, cli_addr;

void sig_killhandler(int signo);
void logData(char *fileName, char *mode);
void createLogFile(char *fileName, char *mode);

int main(int argc, char *argv[])
{   
    /* Creating a 'Log File' to record the process information like 'PID' and 'Last Activity Time' */
    createLogFile("./server.txt","w+");
    /* Logging the data */
    logData("server.txt","w+");
    // A 'SIGINT' signal from the 'watchdog process' to the 'motorx process'.
    signal(SIGINT, sig_killhandler);

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        close(pipefd[1]);  // Close unused write end

        // Connect the read end of the pipe to stdin
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);  // Close the read end

        // Your server code here
        // Creates a socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            exit(EXIT_FAILURE);
        }

        // Configure the server address
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(3500);

        // Bind the socket to the IP address
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR on binding");
            exit(EXIT_FAILURE);
        }

        // Server is ready! Waits for new client to request
        printf("Server is ready! Waits for new client to request...\n");
        listen(sockfd, 5);

        // Accepts the request
        clilent = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilent);
        printf("Accepts the request\n");

        // Error Handling
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(EXIT_FAILURE);
        }

        close(newsockfd);  // Close client socket
        close(sockfd);      // Close server socket
    } else {  // Parent process
        close(pipefd[0]);  // Close unused read end

        // Write data to the pipe
        const char *data = "Data to be sent to child process\n";
        if (write(pipefd[1], data, strlen(data)) != strlen(data)) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]);  // Close write end of pipe

        // Wait for the child process to terminate
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

/*
Defining the signal handlers function
*/
void sig_killhandler(int signo) {
    if (signo == SIGINT) {
        printf("I Received SIGINT Signal!\n");
        exit(EXIT_SUCCESS);
    }
}

/*a function to create a log file for the target process*/
void createLogFile(char *fileName, char *mode) {
    /*In a new run of the program, if the log file exists, remove the old log file and create a new one*/
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

/*a function to record the essential information of a process into a logfile.*/
void logData(char *fileName, char *mode) {
    FILE *fp;
    int pid = getpid();

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    /*Opening the logfile and record the current activity time of the process*/
    fp = fopen(fileName, mode);
    /*Error Checking*/ 
    if (fp<0){
        printf("Could not open the %s file; errno=%d\n", fileName, errno);
        exit(1); 
    }
    /*Writing into the file*/ 
    fprintf(fp, "%d,%d,%d,%d\n", pid, tm.tm_hour, tm.tm_min, tm.tm_sec);
    /*Closing the file*/
    fclose(fp);
}
