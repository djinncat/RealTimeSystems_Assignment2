/* **********************************************
* Pick and Place Program
* Startup and Monitoring main file
* By: Kate Bowater
* Student #: U1019160
*
* This file creates forks and pipes for communication
* between the Display, Simulator, and Controller
*
************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define NUMBER_OF_CHILDREN 1
#define CHILD 0
#define FORK_FAILED -1
#define READ 0
#define WRITE 1

//#define READ_BLOCK_SIZE 9

int pipe_Startup_to_Display[2];  //[0] for read, [1] for write

int main()
{
    //pipe details
    //char readBuffer[READ_BLOCK_SIZE+1];
    //ssize_t bytesRead;

    char *strFromStartup = "Here is a newer, longer message";
    char pipeStartupToDisplayReadFdStr[10];
    //end pipe details

    int DisplayStatus; //for parent to monitor the status of the child later on

    //set up pipe before fork
    if (pipe(pipe_Startup_to_Display) < 0)
    {
        perror("Pipe creation failed");
        exit(5);
    }
    // string creation for read end of pipe, to allow display to read from startup
    sprintf(pipeStartupToDisplayReadFdStr, "%d", pipe_Startup_to_Display[READ]);

    for (int count = 0; count < NUMBER_OF_CHILDREN; count++)
    {
        pid_t return_pid = fork();
        //using switch to organise the child and parent processes
        switch (return_pid)
        {
            case CHILD:
                if (count == 0)  //first is the display process
                {
                    close(pipe_Startup_to_Display[WRITE]); //display will only be reading through the pipe
                    printf("Display child created with PID %d. Going to overlay.\n", getpid());
                    char countString[2];
                    sprintf(countString, "%i", count + 1);
                    execl("..\\Assgn2_2024_Display\\bin\\Release\\Assgn2_2024_Display", "Assgn2_2024_Display", pipeStartupToDisplayReadFdStr, (char *) NULL);
                    perror("Display overlay failed: ");
                    exit(5);
                }


            case FORK_FAILED:
                perror("Fork failed: ");
                exit(5);

            // the Startup process continues here as the parent
            default:
                printf("Startup process has created child.\n");
                close(pipe_Startup_to_Display[READ]); //parent will only write to pipe, so close read end
                printf("Startup writing to pipe\n");
                write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
                printf("Startup closing pipe\n");
                close(pipe_Startup_to_Display[WRITE]); // finished writing to the pipe
//                bytesRead = read(pipe_Startup_to_Display[READ], readBuffer, READ_BLOCK_SIZE);  //startup will stop here until there is a message
//                printf("Startup process reading message:\n");
//                while (bytesRead > 0)
//                {
//                    readBuffer[bytesRead] = '\0';
//                    printf("bytes:%li\n'%s'\n", bytesRead, readBuffer);
//                    bytesRead = read(pipe_Startup_to_Display[READ], readBuffer, READ_BLOCK_SIZE);
//                }

                printf("Startup now waiting for display child to exit\n");
                pid_t DisplayPID = wait(&DisplayStatus); // waiting for the display process to exit
                printf("Startup: Display with PID %d terminated, status code %d.\n", DisplayPID, DisplayStatus>>8);


        }     // end switch
    }//end for loop
    printf("Process spawning completed\n");
    exit(0);
}//end main
