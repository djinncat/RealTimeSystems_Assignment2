#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pnpDisplay.h"

#define READ_BLOCK_SIZE 50

PnP *pnp;
//this function gets the simulation time from the shared pnp file that simulator and controller also access
double simulation_time()
{
    return pnp->sim_time;
}


int main(int argc, char *argv[])
{
    //opening a mapped memory file to share with simulator and controller in order to get simulation time
    int fd = open(MEMORY_MAPPED_FILE, (O_CREAT | O_RDWR), 0666);
    if (fd < 0)
    {
        perror("creation/opening of memory mapped file failed");
        exit(1);
    }
    ftruncate(fd, sizeof(PnP));

    /* map the file to memory */
    pnp = (PnP *)mmap(0, sizeof(PnP), (PROT_READ | PROT_WRITE), MAP_SHARED, fd, (off_t)0);
    if (pnp == MAP_FAILED)
    {
        perror("memory mapping of file failed");
        close(fd);
        exit(2);
    }

    char readBufferStartup[READ_BLOCK_SIZE+1], readBufferSim[READ_BLOCK_SIZE+1], readBufferContrl[READ_BLOCK_SIZE+1];
    ssize_t bytesReadStartup, bytesReadSim, bytesReadContrl;

    // set up the file descriptors for each of the other processes to communicate
    int readStartupFd = atoi(argv[1]);
    int readSimFd = atoi(argv[2]);
    int readContrlFd = atoi(argv[3]);

    printf("DISPLAY\nTime: %7.2f  Now reading from pipes\n", simulation_time());
    //display process will stop here until there is a message to be read
    bytesReadStartup = read(readStartupFd, readBufferStartup, READ_BLOCK_SIZE);
    bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
    bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);

    //stays in loop as long as there are bytes in the pipes for reading
    while (bytesReadStartup > 0 || bytesReadSim > 0 || bytesReadContrl > 0)
    {
        if (bytesReadStartup > 0) //for the pipe connected to Startup process
        {
            printf("STARTUP\n");
            while(readBufferStartup[bytesReadStartup-1]!='\n')
            {  // continue printing until the new line is specified
                readBufferStartup[bytesReadStartup] = '\0';
                printf("%s", readBufferStartup);
                bytesReadStartup = read(readStartupFd, readBufferStartup, READ_BLOCK_SIZE);
            }

            readBufferStartup[bytesReadStartup] = '\0';
            printf("%s", readBufferStartup);
            bytesReadStartup = read(readStartupFd, readBufferStartup, READ_BLOCK_SIZE);
        }

        //for the pipes connected to simulator and controller
        if (bytesReadSim > 0 || bytesReadContrl > 0)
        {   //use strncmp to check if the messages contains 'Time' at the beginning of the strings. This is to
            //ensure that messages that do not have a simulation time are not broken up
            if(strncmp("Time", readBufferSim, 3) == 0 && strncmp("Time", readBufferContrl, 3) == 0)
            { //Do comparison between simulation time to determine which came first
                //strncmp will return <0 if readBufferSim is lower in value to readBufferContrl for the first 20 chars
                //i.e. simulator has earlier simulation time
                if(strncmp(readBufferSim, readBufferContrl, 20) < 0)
                {
                    printf("SIMULATOR\n");
                    while (readBufferSim[bytesReadSim-1] != '\n')
                    {
                        readBufferSim[bytesReadSim] = '\0';
                        printf("%s", readBufferSim);
                        bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
                    }
                    readBufferSim[bytesReadSim] = '\0';
                    printf("%s", readBufferSim);
                    bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
                }

                // strncmp will return >0 if readBufferSim is higher in value to readBufferContrl for the first 20 chars
                // i.e. the controller will have earlier simulation time
                if (strncmp(readBufferSim, readBufferContrl, 20) > 0)
                {
                    printf("CONTROLLER\n");
                    while (readBufferContrl[bytesReadContrl-1] != '\n')
                    {
                        readBufferContrl[bytesReadContrl] = '\0';
                        printf("%s", readBufferContrl);
                        bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);
                    }
                    readBufferContrl[bytesReadContrl] = '\0';
                    printf("%s", readBufferContrl);
                    bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);
                }

            }

            else
            { // if the message from simulator or controller does not contain 'Time' or simulation time, print as usual
                if (bytesReadSim > 0)
                {
                    printf("SIMULATOR\n");
                    while (readBufferSim[bytesReadSim-1] != '\n')
                    {
                        readBufferSim[bytesReadSim] = '\0';
                        printf("%s", readBufferSim);
                        bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
                    }
                    readBufferSim[bytesReadSim] = '\0';
                    printf("%s", readBufferSim);
                    bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
                }

                if (bytesReadContrl > 0)
                {
                    printf("CONTROLLER\n");
                    while (readBufferContrl[bytesReadContrl-1] != '\n')
                    {
                        readBufferContrl[bytesReadContrl] = '\0';
                        printf("%s", readBufferContrl);
                        bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);
                    }
                    readBufferContrl[bytesReadContrl] = '\0';
                    printf("%s", readBufferContrl);
                    bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);
                }
            }
        } //end if, checking for bytesreadSim and bytesReadContrl

    } //end while loop


    // the writing ends of the pipes have been closed and there are no more bytes to read
    printf("DISPLAY\nTime: %7.2f  Finished reading from pipe\n", simulation_time());
    printf("DISPLAY\nTerminating...\n");
    munmap(pnp, sizeof(PnP)); //unmap the shared file PnP
    close(fd);
    close(readStartupFd);  //close all the pipes and terminate
    close(readSimFd);
    close(readContrlFd);
    exit(10);

} // end main


/* **********************************************
SEMAPHORE SETUP STUFF


#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define NUM_TIMES 5


int main()
{
    sem_t *sem_child = sem_open("/sem_child", O_CREAT, 0666, 1);
    sem_t *sem_parent = sem_open("/sem_parent", O_CREAT, 0666, 0);

    pid_t return_pid = fork();

    if (return_pid == 0)
    {

        for (int i = 0; i < NUM_TIMES; i++)
        {
            sem_wait(sem_child);
            printf("this\n");
            sem_post(sem_parent);
        }

        exit(0);
    }
    else
    {
        for (int i = 0; i < NUM_TIMES; i++)
        {
            sem_wait(sem_parent);
            printf("that\n");
            sem_post(sem_child);
        }

    }
    wait(NULL);
    sem_close(sem_child);
    sem_close(sem_parent);
    sem_unlink("/sem_child");
    sem_unlink("/sem_parent");
}
***********************************************
*/
