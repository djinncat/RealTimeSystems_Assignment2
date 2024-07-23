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

int pipe_Startup_to_Display[2];  //[0] for read, [1] for write
int pipe_Simulator_to_Display[2];
int pipe_Controller_to_Display[2];

int main()
{
    //named semaphore creation
    sem_t *sem_Startup = sem_open("/sem_Startup", O_CREAT, 0666, 1);
    sem_t *sem_Sim = sem_open("/sem_Sim", O_CREAT, 0666, 1);

    if (sem_Startup == SEM_FAILED || sem_Sim == SEM_FAILED)
    {
        perror("Semaphore creation failed");
        exit(6);
    }

    char *strFromStartup;
    char Startup_str_array[100];
    char pipeStartupToDisplayReadFdStr[10];  //used to allow display end of pipe to read from startup process
    char pipeSimToDisplayReadFdStr[10];  //used to allow display end of pipe to read from simulator process
    char pipeContrlToDisplayReadFdStr[10];  //used to allow display end of pipe to read from controller process
    char pipeSimToDisplayWriteFdStr[10];  // used to allow simulator to write to display pipe
    char pipeContrlToDisplayWriteFdStr[10]; // used to allow controller to write to display pipe

    int Status; //for parent to monitor the status of child

    //set up pipes before fork
    if (pipe(pipe_Startup_to_Display) < 0 || pipe(pipe_Simulator_to_Display) < 0 || pipe(pipe_Controller_to_Display) < 0)
    {
        perror("Pipe creation failed");
        exit(5);
    }
    //setting up pipe to be non-blocking so display can continue read from other pipes
    if (fcntl(pipe_Startup_to_Display[READ], F_SETFL, O_NONBLOCK) < 0) {
        perror("Pipe non-blocking failed");
        exit(5);
    }

    // string creation for read end of pipe, to allow overlayed process to read or write
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
                    execl("..\\Assgn2_2024_Display\\bin\\Release\\Assgn2_2024_Display", "Assgn2_2024_Display", pipeStartupToDisplayReadFdStr,
                          pipeSimToDisplayReadFdStr, pipeContrlToDisplayReadFdStr, (char *) NULL);
                    perror("Display overlay failed");
                    exit(5);
                }

                if(count == 1)  // second child is the simulator
                {
                    sprintf(Startup_str_array, "Simulator process created with PID %d\n", getpid());
                    write(pipe_Startup_to_Display[WRITE], Startup_str_array, strlen(Startup_str_array));
                    close(pipe_Startup_to_Display[WRITE]);  //Simulator overlayed does not need this pipe
                    close(pipe_Simulator_to_Display[READ]);  //Simulator will only write to pipe
                    //close(pipe_Startup_to_Display[READ]);  // does not need access to the startup pipe
                    close(pipe_Controller_to_Display[READ]);  //does not need access to the controller pipe
                    close(pipe_Controller_to_Display[WRITE]);
                    execl("..\\Assgn2_2024_Simulator\\bin\\Release\\Assgn2_2024_Simulator", "Assgn2_2024_Simulator", pipeSimToDisplayWriteFdStr, (char *) NULL);
                    perror("Simulator overlay failed");
                    exit(5);
                }

                if(count == 2)  // final child is the Controller
                {
                    sprintf(Startup_str_array, "Controller process created with PID %d\n", getpid());
                    write(pipe_Startup_to_Display[WRITE], Startup_str_array, strlen(Startup_str_array));
                    close(pipe_Startup_to_Display[WRITE]);  //Controller process overlayed does not need this pipe
                    close(pipe_Controller_to_Display[READ]);  //Controller will only write to pipe
                    //close(pipe_Simulator_to_Display[READ]);  //does not need access to the simulator pipe
                    //close(pipe_Simulator_to_Display[WRITE]);
                    execl("..\\Assgn2_2024_Controller\\bin\\Release\\Assgn2_2024_Controller", "Assgn2_2024_Controller", pipeContrlToDisplayWriteFdStr, (char *) NULL);
                    perror("Controller overlay failed");
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
                }

                if(count == 1) //after spawning simulator
                {
                    close(pipe_Simulator_to_Display[READ]);  //does not need access to the simulator pipe
                    close(pipe_Simulator_to_Display[WRITE]);
                }

                if(count == 2)  // after spawning controller
                {
                    close(pipe_Controller_to_Display[READ]);  //does not need access to the controller pipe
                    close(pipe_Controller_to_Display[WRITE]);
                }

        }     // end switch
    }//end for loop

    strFromStartup = "Process spawning complete\n";
    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
    strFromStartup = "Waiting for children to terminate\n";
    write(pipe_Startup_to_Display[WRITE], strFromStartup, strlen(strFromStartup));
    sem_post(sem_Startup); // Other processes wait for this to prevent race conditions

   // waiting for children to terminate. Prints the PID and exit status of each.
   // The controller always terminates first followed by Simulator
    pid_t Contrl_pid = wait(&Status);
    sprintf(Startup_str_array, "Controller with PID %d terminated with status code %d\n", Contrl_pid, Status>>8);
    write(pipe_Startup_to_Display[WRITE], Startup_str_array, strlen(Startup_str_array));
    pid_t Sim_pid = wait(&Status);
    sprintf(Startup_str_array, "Simulator with PID %d terminated with status code %d\n", Sim_pid, Status>>8);
    write(pipe_Startup_to_Display[WRITE], Startup_str_array, strlen(Startup_str_array));

    // Display pipe needs to be closed to allow Display process to terminate
    close(pipe_Startup_to_Display[WRITE]);
    pid_t Display_pid = wait(&Status);
    printf("STARTUP\nDisplay with PID %d terminated with status code %d\n", Display_pid, Status>>8);
    printf("STARTUP\nProgram has ended. Press any key to exit.\n");
    sem_close(sem_Startup);
    sem_unlink("/sem_Startup");
    sem_close(sem_Sim);
    sem_unlink("/sem_Sim");
    exit(0);
}//end main
