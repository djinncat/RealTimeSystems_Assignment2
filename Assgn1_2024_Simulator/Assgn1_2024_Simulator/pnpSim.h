/*
 *
 * pnpSim.h - declarations for pick and place machine simulator
 *
 * Platform: Any POSIX compliant platform
 * Intended for and tested on: Cygwin 64 bit
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define MEMORY_MAPPED_FILE "pnp_shared_file"

#define MAX_NUMBER_OF_COMPONENTS_TO_PLACE 100

#define HOME_X 0.0
#define HOME_Y 0.0

#define MIN_X -200.0
#define MIN_Y -200.0
#define MAX_X +1000.0
#define MAX_Y +1000.0

#define NUMBER_OF_FEEDERS 10
#define NO_PICKED_PART -1
#define NO_TAPE_FEEDER_AT_THIS_LOCATION -1
#define FDR_0_X +50.0
#define FDR_1_X +150.0
#define FDR_2_X +250.0
#define FDR_3_X +350.0
#define FDR_4_X +450.0
#define FDR_5_X +550.0
#define FDR_6_X +650.0
#define FDR_7_X +750.0
#define FDR_8_X +850.0
#define FDR_9_X +950.0
#define FDR_0_Y -100.0
#define FDR_1_Y -100.0
#define FDR_2_Y -100.0
#define FDR_3_Y -100.0
#define FDR_4_Y -100.0
#define FDR_5_Y -100.0
#define FDR_6_Y -100.0
#define FDR_7_Y -100.0
#define FDR_8_Y -100.0
#define FDR_9_Y -100.0

#define LOOKUP_CAMERA_X -100
#define LOOKUP_CAMERA_Y +100
#define PHOTO_LOOKUP 0
#define PHOTO_LOOKDOWN 1
#define MAX_THETA_PICK_MISALIGNMENT 10   // maximum of +or-5 degrees misalignment
#define MAX_X_PREPLACE_MISALIGNMENT 20   // maximum of +or-10 units misalignment
#define MAX_Y_PREPLACE_MISALIGNMENT 20   // maximum of +or-10 units misalignment

#define POLL_LOOP_RATE 100               // poll loops per second - must be more than the controller

#define TRUE 1
#define FALSE 0

#define NUMBER_OF_NOZZLES 3
#define LEFT_NOZZLE 0
#define CENTRE_NOZZLE 1
#define RIGHT_NOZZLE 2

#define NOZZLE_X_SEPARATION 20

#define NO_INSTRUCTION 0
#define MOVE_HEAD 1
#define ROTATE_NOZZLE 2
#define LOWER_NOZZLE 3
#define RAISE_NOZZLE 4
#define APPLY_VACUUM 5
#define RELEASE_VACUUM 6
#define TAKE_PHOTO 7
#define AMEND_HEAD_POSITION 8

#define HEAD_FULL_SPEED 1000.0    // 1000 units per second
#define NOZZLE_ROTATE_SPEED 360.0 // 360 degrees per second
#define NOZZLE_LOWER_TIME 0.1     // 0.1 seconds
#define NOZZLE_RAISE_TIME 0.1     // 0.1 seconds
#define VACUUM_APPLY_TIME 0.05    // 0.05 seconds
#define VACUUM_RELEASE_TIME 0.05  // 0.05 seconds
#define PHOTO_TAKE_TIME 0.05      // 0.05 seconds

typedef struct
{
    int ready_for_next_instruction;
    double sim_time;
    double theta_pick_error[NUMBER_OF_NOZZLES];
    double x_preplace_error;
    double y_preplace_error;
    int instruction_to_execute;
    double instruction_argument_1;
    double instruction_argument_2;
    int instruction_argument_3;
    int quit;

} PnP;

typedef struct
{
    double x_actual;
    double y_actual;
    double theta_actual;
    int feeder;

} PlacedPart;

void resetPnP(PnP*, double);

void sleepMilliseconds(long ms);

int getTapeFeederNumberAtLocation(double, double);



