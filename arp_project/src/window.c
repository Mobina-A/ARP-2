#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <errno.h>


#define BUFFER_SIZE 1024

int main() {
    int pipe_fd[2]; // File descriptors for the pipe
    pid_t child_pid; // PID of the child process
    char buffer[BUFFER_SIZE]; // Buffer for reading from the pipe

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    child_pid = fork();

    if (child_pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) { // Child process (UI process)
        close(pipe_fd[1]); // Close the write end of the pipe

        // Start the ncurses window
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0); // Hide cursor
        timeout(200); // Set a timeout for getch() to simulate real-time behavior

        // Read from the pipe and update the UI accordingly
        while (read(pipe_fd[0], buffer, BUFFER_SIZE) > 0) {
            // Update the UI based on the data received from the pipe
            clear(); // Clear the screen
            mvprintw(0, 0, "Received data: %s", buffer); // Print received data
            refresh(); // Refresh the screen
        }

        // End ncurses and close the read end of the pipe
        endwin();
        close(pipe_fd[0]);
    } else { // Parent process
        close(pipe_fd[0]); // Close the read end of the pipe

        #include <unistd.h> // Include for usleep()

	// Simulate updating data in the parent process and writing it to the pipe
	int counter = 0;
	while (1) {
   	 	// Update data as needed
   	 	snprintf(buffer, BUFFER_SIZE, "Data update %d", counter++);

    		// Write data to the pipe
    		if (write(pipe_fd[1], buffer, BUFFER_SIZE) == -1) {
        	perror("Write to pipe failed");
        	exit(EXIT_FAILURE);
    		}

    	// Sleep for a while to simulate periodic updates
    	usleep(500000); // 0.5 second delay
	}


        close(pipe_fd[1]); // Close the write end of the pipe
    }

    return 0;
}
