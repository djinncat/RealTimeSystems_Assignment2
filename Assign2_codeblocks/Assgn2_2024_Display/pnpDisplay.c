#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define READ_BLOCK_SIZE 50



int main(int argc, char *argv[])
{

    char readBufferStartup[READ_BLOCK_SIZE+1], readBufferSim[READ_BLOCK_SIZE+1], readBufferContrl[READ_BLOCK_SIZE+1];
    ssize_t bytesReadStartup, bytesReadSim, bytesReadContrl;

    // set up the file descriptors for each of the other processes to communicate
    int readStartupFd = atoi(argv[1]);
    int readSimFd = atoi(argv[2]);
    int readContrlFd = atoi(argv[3]);

    printf("DISPLAY\nNow reading and printing from pipes\n");
    //display process will stop here until there is a message to be read
    bytesReadSim = read(readSimFd, readBufferSim, READ_BLOCK_SIZE);
    bytesReadContrl = read(readContrlFd, readBufferContrl, READ_BLOCK_SIZE);

    while(1)
    {	// check if there are bytes in the Startup buffer
        bytesReadStartup = read(readStartupFd, readBufferStartup, READ_BLOCK_SIZE);

        if (bytesReadStartup > 0) //if there are bytes to read
        {
            printf("STARTUP\n");
            while(readBufferStartup[bytesReadStartup-1]!='\n') //keep printing until new line
            {  // continue printing until the new line is specified
                readBufferStartup[bytesReadStartup] = '\0';
                printf("%s", readBufferStartup);
                bytesReadStartup = read(readStartupFd, readBufferStartup,READ_BLOCK_SIZE);
            }
            //once a new line detected, print the remaining part of the message
            readBufferStartup[bytesReadStartup] = '\0';
            printf("%s", readBufferStartup);
            bytesReadStartup = read(readStartupFd, readBufferStartup, READ_BLOCK_SIZE);
        }

        // otherwise if all pipes are closed, terminate the Display process
        else if (bytesReadStartup == 0 && bytesReadSim == 0 && bytesReadContrl == 0)
        {
	       printf("DISPLAY\nFinished reading from pipes\nTerminating...\n");
            close(readStartupFd);  //close all the pipes and terminate
            close(readSimFd);
            close(readContrlFd);
            exit(10);
        }
        // if there are no bytes from Startup, continue with program

        //i.e. simulator has earlier simulation time
        if(strncmp("Time", readBufferSim, 3) == 0 && strncmp("Time", readBufferContrl, 3) == 0)
        { //Do comparison between simulation time to determine which came first
          //strncmp will return <0 if readBufferSim is lower in value to readBufferContrl for the first 20 chars
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
		//might need to add another byte read here to compare next message sim time
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

    }//end while loop
} // end main
