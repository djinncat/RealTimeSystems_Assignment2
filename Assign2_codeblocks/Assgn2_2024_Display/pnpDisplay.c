#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define READ_BLOCK_SIZE 25

int main(int argc, char *argv[])
{
    char readBuffer[READ_BLOCK_SIZE+1];
    ssize_t bytesRead;
    //char *str = "Message through pipe";
    int readFd;

    printf("I am Display child printing on my own with PID %d\n", getpid());

    readFd = atoi(argv[1]);
    //sleep(4);
    //printf("Display: I will send message to Startup Process\n");
    //write(writeFd, str, strlen(str));
    printf("Display: I will read from the pipe\n");
    bytesRead = read(readFd, readBuffer, READ_BLOCK_SIZE);  //display process will stop here until there is a message
    printf("Display process reading message:\n");
    while (bytesRead > 0)
    {
        readBuffer[bytesRead] = '\0';
        printf("bytes:%li\n'%s'\n", bytesRead, readBuffer);
        bytesRead = read(readFd, readBuffer, READ_BLOCK_SIZE);
    }

    printf("Display: Finished reading from pipe\n");
    close(readFd);
    exit(10);
}
