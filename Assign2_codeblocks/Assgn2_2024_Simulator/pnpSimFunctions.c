/*
 *
 * pnpSimFunctions.c - provides support functions for pnpSim.c
 *
 * Platform: Any POSIX compliant platform
 * Intended for and tested on: Cygwin 64 bit
 *
 */

#include "pnpSim.h"

/*
 Function: resetPnP
 ------------------
 Written by Jason Brown
 Date: 30/03/2024
 Version 1.0
 Purpose: resets the fields of a PnP struct
 Argument(s):
 PnP *pnp - pointer to the pick and place machine system to be reset
 double init_sim_time - initial simulation time
 Return Value: none
 Usage: resetPnP(pnp, sim_time, operationMode);
 */
void resetPnP(PnP *pnp, double init_sim_time)
{

    pnp -> sim_time = init_sim_time;
    pnp -> ready_for_next_instruction = TRUE;
    for (int i = 0; i < NUMBER_OF_NOZZLES; i++)
    {
        pnp -> theta_pick_error[i] = 0.0;
    }
    pnp -> x_preplace_error = 0.0;
    pnp -> y_preplace_error = 0.0;
    pnp -> instruction_to_execute = NO_INSTRUCTION;
    pnp -> instruction_argument_1 = 0.0;
    pnp -> instruction_argument_2 = 0.0;
    pnp -> instruction_argument_3 = 0;
    pnp -> quit = FALSE;

}

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

/*
 Function: getTapeFeederNumberAtLocation
 ---------------------------------------
 Written by Jason Brown
 Date: 30/03/2024
 Version 1.0
 Purpose: find the number of the tape feeder at the location with
 co-ordinates (x,y), else return NO_TAPE_FEEDER_AT_THIS_LOCATION (-1)
 if there is no tape feeder at
 that location
 Argument(s):
 double x - x co-ordinate
 double y - y co-ordinate
 Return Value: the number of the tape feeder at the location with
 co-ordinates (x,y), else NO_TAPE_FEEDER_AT_THIS_LOCATION (-1)
 if there is no tape feeder at that location
 Usage: int tape_feed_number = getTapeFeederNumberAtLocation(x,y);
 */
int getTapeFeederNumberAtLocation(double x, double y)
{

    const double TAPE_FEEDER_X[NUMBER_OF_FEEDERS] = {FDR_0_X, FDR_1_X, FDR_2_X, FDR_3_X, FDR_4_X, FDR_5_X, FDR_6_X, FDR_7_X, FDR_8_X, FDR_9_X};
    const double TAPE_FEEDER_Y[NUMBER_OF_FEEDERS] = {FDR_0_Y, FDR_1_Y, FDR_2_Y, FDR_3_Y, FDR_4_Y, FDR_5_Y, FDR_6_Y, FDR_7_Y, FDR_8_Y, FDR_9_Y};

    for (int i = 0; i < NUMBER_OF_FEEDERS; i++)
    {

        if (x == TAPE_FEEDER_X[i] && y == TAPE_FEEDER_Y[i]) return i;

    }
    return NO_TAPE_FEEDER_AT_THIS_LOCATION;

}
