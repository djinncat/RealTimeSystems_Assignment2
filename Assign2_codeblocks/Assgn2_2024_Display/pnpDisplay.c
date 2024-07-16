#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define READ_BLOCK_SIZE 100

int main(int argc, char *argv[])
{

    char readStartupBuffer[READ_BLOCK_SIZE+1];
    char readSimBuffer[READ_BLOCK_SIZE+1];
    ssize_t bytesRead1, bytesRead2;

    printf("I am Display child printing on my own with PID %d\n", getpid());

    int readStartupFd = atoi(argv[1]);
    int readSimFd = atoi(argv[2]);
    bytesRead1 = read(readStartupFd, readStartupBuffer, READ_BLOCK_SIZE);  //display process will stop here until there is a message
    bytesRead2 = read(readSimFd, readSimBuffer, READ_BLOCK_SIZE);
    printf("Display: I will receive and display all messages from now on\n");
//switch case might solve problem here

    while (bytesRead1 > 0 || bytesRead2 > 0)
    {
        readStartupBuffer[bytesRead1] = '\0';
        readSimBuffer[bytesRead2] = '\0';
        printf("%s", readStartupBuffer);
        printf("%s", readSimBuffer);
        bytesRead1 = read(readStartupFd, readStartupBuffer, READ_BLOCK_SIZE);
        bytesRead2 = read(readSimFd, readSimBuffer, READ_BLOCK_SIZE);
    }

    printf("Display: Finished reading from pipes\n");
    close(readStartupFd);
    close(readSimFd);
    exit(10);
}


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
