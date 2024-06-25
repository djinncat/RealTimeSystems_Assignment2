/* **********************************************
* Pick and Place Program
* Display main file
* Date edited: 25 June 2024
* By: Kate Bowater
* Student #: U1019160
*
* This file prints strings provided by the Startup,
* Simulator and Controller processes.
*
************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{;
    printf("Display PID: %i\n", dgetpid());
    exit(1);
}
