/* **********************************************
* Pick and Place Program
* Startup and Monitoring main file
* Date edited: 25 June 2024
* By: Kate Bowater
* Student #: U1019160
*
* This file creates forks and points the child processes to
* the Display, Simulator, or Controller
*
************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t return_result =  fork();
    if (return_result == 0)
    {
        execl("..\\Assgn2_2024_Display\\bin\\release\\Assgn2_2024_Display", "Assgn2_2024_Display.exe", (char *)NULL);
        exit(1);
    }
    else if (return_result < 0)
    {
        perror("Fork failed");
        exit(1);
    }
}
