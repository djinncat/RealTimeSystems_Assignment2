#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pnpDisplay.h"

#define READ_BLOCK_SIZE 200

int main(int argc, char *argv[])
{
    PnP *pnp;
        /* initialize file for memory mapping */
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
    printf("I am Display child printing on my own with PID %d\n", getpid());
    // set up the file descriptors for each of the other processes to communicate
    int readStartupFd = atoi(argv[1]);
    int readSimFd = atoi(argv[2]);
    int readContrlFd = atoi(argv[3]);

    bytesReadStartup = read(readStartupFd, readBufferStartup, READ_BLOCK_SIZE);  //display process will stop here until there is a message
    bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
    bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);

    printf("Display: I will receive and display all messages from now on\n");

    while (bytesReadStartup > 0 || bytesReadSim > 0 || bytesReadContrl > 0)
    {
        if (bytesReadStartup > 0)
        {
            readBufferStartup[bytesReadStartup] = '\0';
            printf("Startup: %s", readBufferStartup);
            bytesReadStartup = read(readStartupFd, readBufferStartup, READ_BLOCK_SIZE);
        }

        if (bytesReadContrl > 0)
        {
            readBufferContrl[bytesReadContrl] = '\0';
            printf("Controller: %s", readBufferContrl);
            bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);
        }

        if (bytesReadSim > 0)
        {
            readBufferSim[bytesReadSim] = '\0';
            printf("Simulator: %s", readBufferSim);
            bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
        }

    } //end while loop

    // the writing end of the pipe has been closed and there are no
    // more bytes to read
    printf("Display: Finished reading from pipe\n");
    printf("Display: Terminating...\n");
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
