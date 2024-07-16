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
#include <fcntl.h>

#define NUMBER_OF_CHILDREN 1
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

int main()
{
    //sem_t *sem_child = sem_open("/sem_child", O_CREAT, 0666, 1);
    //sem_t *sem_parent = sem_open("/sem_parent", O_CREAT, 0666, 0);

    //pipe details

    char *strFromStartup;  //will contain messages from Startup
    char pipeStartupToDisplayReadFdStr[10];  //used to allow display end of pipe to read from startup process
    char pipeSimToDisplayReadFdStr[10];  //used to allow display end of pipe to read from simulator process
    char pipeSimToDisplayWriteFdStr[10];  //used to allow simulator to write to the display process

    //end pipe details

    int DisplayStatus; //for parent to monitor the status of the display child
//    int SimulatorStatus; //for parent to monitor the status of the simulator child

    //set up pipe before fork
    if (pipe(pipe_Startup_to_Display) < 0 || pipe(pipe_Simulator_to_Display) < 0)
    {
        perror("Pipe creation failed");
        exit(5);
    }
    //setting up display pipes to be non-blocking to read from multiples pipes
    if (fcntl(pipe_Startup_to_Display[READ], F_SETFL, O_NONBLOCK) < 0 || fcntl(pipe_Simulator_to_Display[READ], F_SETFL, O_NONBLOCK)<0) {
        perror("Pipe non-blocking fail");
        exit(5);
    }

    // string creation for read end of pipe, to allow process to read or write
    sprintf(pipeStartupToDisplayReadFdStr, "%d", pipe_Startup_to_Display[READ]);
    sprintf(pipeSimToDisplayReadFdStr, "%d", pipe_Simulator_to_Display[READ]);
    sprintf(pipeSimToDisplayWriteFdStr, "%d", pipe_Simulator_to_Display[WRITE]);

    for (int count = 0; count < NUMBER_OF_CHILDREN; count++)
    {
        pid_t return_pid = fork();
        //using switch to organise the child and parent processes
        switch (return_pid)
        {
            case CHILD:
                if (count == 0)  //first is the display process
                {
                    printf("Display process created with PID %d. Going to overlay\n", getpid());
                    close(pipe_Startup_to_Display[WRITE]); //display will only be reading through the pipes
                    close(pipe_Simulator_to_Display[WRITE]);
                    execl("..\\Assgn2_2024_Display\\bin\\Release\\Assgn2_2024_Display", "Assgn2_2024_Display", pipeStartupToDisplayReadFdStr,
                          pipeSimToDisplayReadFdStr, (char *) NULL);
                    perror("Display overlay failed: ");
                    exit(5);
                }

                if(count == 1)  // second is the simulator
                {
                    printf("Simulator process created with PID %d. Going to overlay\n", getpid());
                    close(pipe_Simulator_to_Display[READ]);  //Simulator will only write to pipe
                    close(pipe_Startup_to_Display[READ]);  // does not need access to the startup pipe
                    close(pipe_Startup_to_Display[WRITE]);
                    //execl("..\\Assgn2_2024_Simulator\\bin\\Release\\Assgn2_2024_Simulator", "Assgn2_2024_Simulator", pipeSimToDisplayWriteFdStr, (char *) NULL);
                    //perror("Simulator overlay failed: ");
                    //exit(5);
                }

                if(count == 2)  // final child is the Controller
                {
                    printf("Controller process created with PID %d. Going to overlay.\n", getpid());
                    execl("..\\Assgn2_2024_Controller\\bin\\Release\\Assgn2_2024_Controller", "Assgn2_2024_Controller", (char *) NULL);
                    perror("Controller overlay failed: ");
                    exit(5);
                }


            case FORK_FAILED:
                perror("Fork failed: ");
                exit(5);

            // the Startup process continues here as the parent
            default:
                if(count == 0) // after spawning display, startup and monitor comes here
                {
                    close(pipe_Startup_to_Display[READ]); //parent will only write to pipe, so close read end
                    strFromStartup = "Startup: Now sending messages through the pipe to Display\n";
                    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
                }

                if(count == 1) //after spawning simulator
                {
                    close(pipe_Simulator_to_Display[READ]);  //does not need access to the simulator pipe
                    close(pipe_Simulator_to_Display[WRITE]);
                    strFromStartup = "Startup: Sending another message via pipe to Display\n";
                    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
                }

                if(count == 2)  // after spawning controller
                {
                    strFromStartup = "Startup: Sending yet another message to Display.\n";
                    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
                }

        }     // end switch
        sleepMilliseconds((long) 1000 / POLL_LOOP_RATE);
    }//end for loop


    //close(pipe_Simulator_to_Display[READ]);  //Startup does not need communication to Simulator
    //close(pipe_Simulator_to_Display[WRITE]); //Startup does not need communication to Simulator
    strFromStartup = "Startup: Process spawning completed\n";
    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
    //close(pipe_Startup_to_Display[WRITE]);


    //Startup will wait for the children to terminate
//    for (int count = 0; count < NUMBER_OF_CHILDREN; count++)
//        {
//            int status;
////            if (count == NUMBER_OF_CHILDREN)
////            {
////                strFromStartup = "Startup closing pipe to Display\n";
////                write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
////                close(pipe_Startup_to_Display[WRITE]); // finished writing to the pipe
////                pid_t return_pid = wait(&status);
////                printf("Startup: Display process with PID %i terminated ", return_pid);
////                printf("with status %i\n", status>>8);
////            }
////            else
////            {
//                pid_t return_pid = wait(&status);
//
//                sprintf(strFromStartup, "Startup: Process with PID %i ", return_pid);
//                write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
//                sprintf(strFromStartup, "terminated with status %i\n", status>>8);
//                write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
//                //printf("Startup: Child process with PID %i terminated ", return_pid);
//                //printf("with status %i\n", status>>8);
////            }
//
//
//        }



//    for (int count = 0; count < NUMBER_OF_CHILDREN; count++)
//    {
        strFromStartup = "Startup: Waiting for processes to terminate\n";
        write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
        close(pipe_Startup_to_Display[WRITE]);
        pid_t DisplayPID = wait(&DisplayStatus); // waiting for the display process to exit
        printf("Startup: Process with PID %d terminated with status code %d.\n", DisplayPID, DisplayStatus>>8);
//    }
    exit(0);
}//end main
