
#define POLL_LOOP_RATE 50          // poll loops per second


/*
 Function: sleepMilliseconds
 ---------------------------
 Written by Jason Brown
 Date: 30/03/2024
 Version 1.0
 Purpose: put the calling thread to sleep
 for a certain number of ms
 Argument(s):
 long ms - the number of ms to sleep
 Return Value: none
 Usage: sleepMilliseconds(20);
 */
void sleepMilliseconds(long ms)
{
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
