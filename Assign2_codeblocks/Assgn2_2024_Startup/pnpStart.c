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
#include <semaphore.h>
#include <fcntl.h>

#define NUMBER_OF_CHILDREN 3
#define CHILD 0
#define FORK_FAILED -1
#define READ 0
#define WRITE 1
#define POLL_LOOP_RATE 50

void sleepMilliseconds(long ms)
{
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

int pipe_Startup_to_Display[2];  //[0] for read, [1] for write
int pipe_Simulator_to_Display[2];
int pipe_Controller_to_Display[2];

int main()
{
    //sem_t *sem_1 = sem_open("/sem_1", O_CREAT, 0666, 1);
    //sem_t *sem_2 = sem_open("/sem_2", O_CREAT, 0666, 0);

//    char *strFromStartup;
    char Startup_str_array[100];
    char pipeStartupToDisplayReadFdStr[10];  //used to allow display end of pipe to read from startup process
    char pipeSimToDisplayReadFdStr[10];  //used to allow display end of pipe to read from simulator process
    char pipeContrlToDisplayReadFdStr[10];  //used to allow display end of pipe to read from controller process
    char pipeSimToDisplayWriteFdStr[10];  // used to allow simulator to write to display pipe
    char pipeContrlToDisplayWriteFdStr[10]; // used to allow controller to write to display pipe

//    int DisplayStatus; //for parent to monitor the status of the display child
    int Status; //for parent to monitor the status of the simulator child

    //set up pipes before fork
    if (pipe(pipe_Startup_to_Display) < 0 || pipe(pipe_Simulator_to_Display) < 0 || pipe(pipe_Controller_to_Display) < 0)
    {
        perror("Pipe creation failed");
        exit(5);
    }
    //setting up pipes to be non-blocking so display can read from multiples pipes
//    if (fcntl(pipe_Startup_to_Display[READ], F_SETFL, O_NONBLOCK) < 0 || fcntl(pipe_Simulator_to_Display[READ], F_SETFL, O_NONBLOCK) <0 || fcntl(pipe_Controller_to_Display[READ], F_SETFL, O_NONBLOCK)<0) {
//        perror("Pipe non-blocking fail");
//        exit(5);
//    }

    // string creation for read end of pipe, to allow process to read or write
    sprintf(pipeStartupToDisplayReadFdStr, "%d", pipe_Startup_to_Display[READ]);
    sprintf(pipeSimToDisplayReadFdStr, "%d", pipe_Simulator_to_Display[READ]);
    sprintf(pipeContrlToDisplayReadFdStr, "%d", pipe_Controller_to_Display[READ]);
    sprintf(pipeSimToDisplayWriteFdStr, "%d", pipe_Simulator_to_Display[WRITE]);
    sprintf(pipeContrlToDisplayWriteFdStr, "%d", pipe_Controller_to_Display[WRITE]);

    for (int count = 0; count < NUMBER_OF_CHILDREN; count++)
    {
        pid_t return_pid = fork();
        //using switch to organise the child and parent processes
        switch (return_pid)
        {
            case CHILD:
                if (count == 0)  //first child is the display process
                {
                    printf("STARTUP\nDisplay process created with PID %d\n", getpid());
                    close(pipe_Startup_to_Display[WRITE]); //display will only be reading through the pipes
                    close(pipe_Simulator_to_Display[WRITE]);
                    close(pipe_Controller_to_Display[WRITE]);
                    execl("..\\Assgn2_2024_Display\\bin\\Release\\Assgn2_2024_Display", "Assgn2_2024_Display", pipeStartupToDisplayReadFdStr, pipeSimToDisplayReadFdStr, pipeContrlToDisplayReadFdStr, (char *) NULL);
                    perror("Display overlay failed: ");
                    exit(5);
/* USED FOR TESTING COMMUNICATION BETWEEN CHILDREN BEFORE OVERLAY WAS SET UP
                    char readBufferStartup[100+1], readBufferSim[100+1], readBufferContrl[100+1];
                    ssize_t bytesReadStartup, bytesReadSim, bytesReadContrl;
                    printf("Display: Waiting to read and print message from pipes\n");
                    bytesReadStartup = read(pipe_Startup_to_Display[READ], readBufferStartup, 100);
                    bytesReadSim = read(pipe_Simulator_to_Display[READ], readBufferSim, 100);
                    bytesReadContrl = read(pipe_Controller_to_Display[READ], readBufferContrl, 100);
                    // if bytesRead is positive, bytes have been read from the pipe
                    while (bytesReadStartup > 0 || bytesReadSim > 0 || bytesReadContrl > 0)
                    {
                        if (bytesReadStartup > 0)
                        {
                            readBufferStartup[bytesReadStartup] = '\0';
                            printf("%s\n", readBufferStartup);
                            bytesReadStartup = read(pipe_Startup_to_Display[READ], readBufferStartup, 100);
                        }
                        if (bytesReadSim > 0)
                        {
                            readBufferSim[bytesReadSim] = '\0';
                            printf("%s\n", readBufferSim);
                            bytesReadSim = read(pipe_Simulator_to_Display[READ], readBufferSim, 100);
                        }
                        if (bytesReadContrl > 0)
                        {
                            readBufferContrl[bytesReadContrl] = '\0';
                            printf("%s\n", readBufferContrl);
                            bytesReadContrl = read(pipe_Controller_to_Display[READ], readBufferContrl, 100);
                        }
                    }
                    // the writing end of the pipe has been closed and there are no
                    // more bytes to read
                    printf("Display: Finished reading from pipe\n");
                    printf("Display: Terminating...\n");
                    close(pipe_Startup_to_Display[READ]); //display will only be reading through the pipes
                    close(pipe_Simulator_to_Display[READ]);
                    close(pipe_Controller_to_Display[READ]);
                    exit(10);
********************************************************************** */
                }

                if(count == 1)  // second child is the simulator
                {
                    //char *strFromSim = "Message through pipe from Sim to Display\n";
                    //char Sim_str_array[100];  // required to use sprintf as pointer will not work
                    sprintf(Startup_str_array, "Simulator process created with PID %d\n", getpid());
                    write(pipe_Startup_to_Display[WRITE], Startup_str_array, strlen(Startup_str_array));
                    close(pipe_Startup_to_Display[WRITE]);
                    close(pipe_Simulator_to_Display[READ]);  //Simulator will only write to pipe
                    //close(pipe_Startup_to_Display[READ]);  // does not need access to the startup pipe
                    close(pipe_Controller_to_Display[READ]);  //does not need access to the controller pipe
                    close(pipe_Controller_to_Display[WRITE]);
                    //printf("Startup:  Simulator Process created with PID %d\n", getpid());
                    execl("..\\Assgn2_2024_Simulator\\bin\\Release\\Assgn2_2024_Simulator", "Assgn2_2024_Simulator", pipeSimToDisplayWriteFdStr, (char *) NULL);
                    perror("Simulator overlay failed: ");
                    exit(5);
//   USED FOR TESTING COMMUNICATION BETWEEN CHILDREN BEFORE OVERLAY WAS SET UP
                    //printf("Simulator: About to write message to pipe\n");
//                    write(pipe_Simulator_to_Display[WRITE], strFromSim, strlen(strFromSim));
//                    strFromSim = "Simulator: Terminating...\n";
//                    write(pipe_Simulator_to_Display[WRITE], strFromSim, strlen(strFromSim));
//                    close(pipe_Simulator_to_Display[WRITE]);
//                    exit(20);

                }

                if(count == 2)  // final child is the Controller
                {
                    //char *strFromContrl = "Message through pipe from Controller to Display\n";
                    //char Contrl_str_array[100];
                    close(pipe_Controller_to_Display[READ]);  //Controller will only write to pipe
                    //close(pipe_Startup_to_Display[READ]);  // does not need access to the startup pipe
                    sprintf(Startup_str_array, "Controller process created with PID %d\n", getpid());
                    write(pipe_Startup_to_Display[WRITE], Startup_str_array, strlen(Startup_str_array));
                    close(pipe_Startup_to_Display[WRITE]);
                    //close(pipe_Simulator_to_Display[READ]);  //does not need access to the simulator pipe
                    //close(pipe_Simulator_to_Display[WRITE]);
                    //printf("Startup:  Controller Process created with PID %d\n", getpid());
                    execl("..\\Assgn2_2024_Controller\\bin\\Release\\Assgn2_2024_Controller", "Assgn2_2024_Controller", pipeContrlToDisplayWriteFdStr, (char *) NULL);
                    perror("Controller overlay failed: ");
                    exit(5);
//  USED TO TEST COMMUNICATION WITH CHILDREN BEFORE OVERLAY WAS SET UP
                    //printf("Controller: About to write message to pipe\n");
//                    write(pipe_Controller_to_Display[WRITE], strFromContrl, strlen(strFromContrl));
//                    strFromContrl = "Controller: Terminating...\n";
//                    write(pipe_Controller_to_Display[WRITE], strFromContrl, strlen(strFromContrl));
//                    close(pipe_Controller_to_Display[WRITE]);
//                    exit(30);
                }


            case FORK_FAILED:
                perror("Fork failed: ");
                exit(5);

            // the Startup process continues here as the parent
            default:
                if(count == 0) // after spawning display, startup and monitor comes here
                {
                    close(pipe_Startup_to_Display[READ]); //parent will only write to pipe, so close read end
                    //strFromStartup = "Startup: Now sending messages through the pipe to Display\n";
                    //write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
                }

                if(count == 1) //after spawning simulator
                {
                    close(pipe_Simulator_to_Display[READ]);  //does not need access to the simulator pipe
                    close(pipe_Simulator_to_Display[WRITE]);
                    //strFromStartup = "Startup: Sending another message via pipe to Display\n";
                    //write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
                }

                if(count == 2)  // after spawning controller
                {
                    close(pipe_Controller_to_Display[READ]);  //does not need access to the controller pipe
                    close(pipe_Controller_to_Display[WRITE]);
                    //strFromStartup = "Startup: Sending yet another message to Display\n";
                    //write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
                }

        }     // end switch
        sleepMilliseconds((long) 1000 / POLL_LOOP_RATE);
    }//end for loop


//    strFromStartup = "Process spawning completed\n";
//    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
//
//    strFromStartup = "Waiting for children to terminate\n";
//    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));

    close(pipe_Startup_to_Display[WRITE]);  // pipe needs to be closed to allow display to print while waiting for children to terminate

    for (int count = 0; count < NUMBER_OF_CHILDREN; count++)
    {
        pid_t return_pid = wait(&Status);
        printf("STARTUP\nProcess with PID %d terminated with status code %d\n", return_pid, Status>>8);
    }
    printf("Startup: Program has ended. Press any key to exit.\n");
    exit(0);
}//end main
