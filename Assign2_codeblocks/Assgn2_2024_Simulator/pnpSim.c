/*
 *
 * pnpSim.c - simulates the pick and place machine operation
 *
 * This program creates a shared memory segment with the controller via a memory mapped file
 *
 * Platform: Any POSIX compliant platform
 * Intended for and tested on: Cygwin 64 bit
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include "pnpSim.h"

int main(int argc, char *argv[])
{
    double sim_time = 0.0;
    printf("Time: %7.2f  Pick and place machine simulation started successfully!\n", sim_time);
    char *strFromSimulator;
    printf("1");
    int writeSimToDisplayFd = atoi(argv[1]);  // the file descriptor to write from Simulator to Display
    printf("2");
    strFromSimulator = "Simulator: Message from simulator through pipe\n";
    printf("3");
    write(writeSimToDisplayFd, strFromSimulator, strlen(strFromSimulator));
    printf("4");
    strFromSimulator = "Simulator: Terminating...\n";
    printf("5");
    write(writeSimToDisplayFd, strFromSimulator, strlen(strFromSimulator));
    printf("6");
    close(writeSimToDisplayFd);
    printf("7");
    exit(20);


//
//    PnP *pnp;
//
//    PlacedPart placedPart[MAX_NUMBER_OF_COMPONENTS_TO_PLACE];
//
//    double sim_time = 0.0, instruction_finish_time = 0.0;
//    double x = HOME_X, y = HOME_Y, x_target = 0.0, y_target = 0.0, x_preplace_error = 0.0, y_preplace_error = 0.0, controller_del_x = 0.0, controller_del_y = 0.0;
//    double theta_pick_error[NUMBER_OF_NOZZLES] = {0.0, 0.0, 0.0}, controller_theta = 0.0, theta_actual[NUMBER_OF_NOZZLES] = {0.0, 0.0, 0.0};
//    int nozzle = CENTRE_NOZZLE;
//    int nozzle_down[NUMBER_OF_NOZZLES] = {FALSE, FALSE, FALSE};
//    int nozzle_vacuum[NUMBER_OF_NOZZLES] = {FALSE, FALSE, FALSE};
//    int nozzle_picked_part[NUMBER_OF_NOZZLES] = {NO_PICKED_PART, NO_PICKED_PART, NO_PICKED_PART};
//    int instruction_being_executed = NO_INSTRUCTION;
//    int number_of_placed_parts = 0, number_of_dropped_parts = 0;
//    int photo_direction;
//
//    srand(time(0));
//
//    /* initialize file for memory mapping */
//    int fd = open(MEMORY_MAPPED_FILE, (O_CREAT | O_RDWR), 0666);
//    if (fd < 0)
//    {
//        perror("creation/opening of memory mapped file failed");
//        exit(1);
//    }
//    ftruncate(fd, sizeof(PnP));
//
//    /* map the file to memory */
//    pnp = (PnP *)mmap(0, sizeof(PnP), (PROT_READ | PROT_WRITE), MAP_SHARED, fd, (off_t)0);
//    if (pnp == MAP_FAILED)
//    {
//        perror("memory mapping of file failed");
//        close(fd);
//        exit(2);
//    }
//
//    /* reset the pick and place machine*/
//    resetPnP(pnp, sim_time);
//
//    printf("Time: %7.2f  Pick and place machine simulation started successfully!\n", sim_time);
//
//    const char nozzle_name[3][10] = {"Left", "Centre", "Right"};
//
//    /*
//     * loop continuously until simulator is to quit
//     * sleep for a short duration (dictated by POLL_LOOP_RATE)
//     * on each loop
//     */
//    while (pnp -> quit == FALSE)
//    {
//        /*
//         * If there is no instruction currently being executed, this code checks whether there
//         * is a new instruction pending from the controller, and if so, determines the instruction
//         * finish time based upon the type of instruction and possibly the parameters of that instruction.
//         *
//         * It also signals that there is currently an instruction being executed back to the controller
//         * so that the controller waits to issue any further instructions.
//         */
//        if (instruction_being_executed == NO_INSTRUCTION)
//        {
//            int new_instruction = pnp -> instruction_to_execute;
//
//            if (new_instruction == MOVE_HEAD)
//            {
//                x_target = pnp -> instruction_argument_1;
//                y_target = pnp -> instruction_argument_2;
//                if (nozzle_down[LEFT_NOZZLE] == FALSE && nozzle_down[CENTRE_NOZZLE] == FALSE && nozzle_down[RIGHT_NOZZLE] == FALSE)
//                {
//                    if (x_target >= MIN_X && x_target <= MAX_X && y_target >= MIN_Y && y_target <= MAX_Y)
//                    {
//                        pnp -> ready_for_next_instruction = FALSE;
//                        pnp -> instruction_to_execute = NO_INSTRUCTION;
//                        instruction_being_executed = MOVE_HEAD;
//                        instruction_finish_time = sim_time + (double)sqrt(pow((x - x_target), 2) + pow((y - y_target), 2)) / HEAD_FULL_SPEED;
//                        printf("Time: %7.2f  Head moving from (%.2f, %.2f) to (%.2f, %.2f)\n", sim_time, x, y, x_target, y_target);
//                    }
//                    else
//                    {
//                        printf("Time: %7.2f  Bad MOVE_HEAD command: destination out of range\n", sim_time);
//                    }
//                }
//                else
//                {
//                    printf("Time: %7.2f  Bad MOVE_HEAD command: one or more nozzles down\n", sim_time);
//                }
//            }
//            else if (new_instruction == ROTATE_NOZZLE)
//            {
//                nozzle = pnp -> instruction_argument_3;
//                if (nozzle >= LEFT_NOZZLE && nozzle <= RIGHT_NOZZLE)
//                {
//                    pnp -> ready_for_next_instruction = FALSE;
//                    pnp -> instruction_to_execute = NO_INSTRUCTION;
//                    instruction_being_executed = ROTATE_NOZZLE;
//                    controller_theta = pnp -> instruction_argument_1;
//                    instruction_finish_time = sim_time + (double)abs(controller_theta) / NOZZLE_ROTATE_SPEED;
//
//                    printf("Time: %7.2f  %s nozzle being rotated by %.2f degrees\n", sim_time, nozzle_name[nozzle], controller_theta);
//                }
//                else
//                {
//                    printf("Time: %7.2f  Bad ROTATE_NOZZLE command: nozzle out of range\n", sim_time);
//                }
//
//            }
//            else if (new_instruction == LOWER_NOZZLE)
//            {
//                nozzle = pnp -> instruction_argument_3;
//                if (nozzle >= LEFT_NOZZLE && nozzle <= RIGHT_NOZZLE)
//                {
//                    pnp -> ready_for_next_instruction = FALSE;
//                    pnp -> instruction_to_execute = NO_INSTRUCTION;
//                    instruction_being_executed = LOWER_NOZZLE;
//                    instruction_finish_time = sim_time + NOZZLE_LOWER_TIME;
//                    printf("Time: %7.2f  %s nozzle being lowered\n", sim_time, nozzle_name[nozzle]);
//                }
//                else
//                {
//                    printf("Time: %7.2f  Bad LOWER_NOZZLE command: nozzle out of range\n", sim_time);
//                }
//            }
//            else if (new_instruction == RAISE_NOZZLE)
//            {
//                nozzle = pnp -> instruction_argument_3;
//                if (nozzle >= LEFT_NOZZLE && nozzle <= RIGHT_NOZZLE)
//                {
//                    pnp -> ready_for_next_instruction = FALSE;
//                    pnp -> instruction_to_execute = NO_INSTRUCTION;
//                    instruction_being_executed = RAISE_NOZZLE;
//                    instruction_finish_time = sim_time + NOZZLE_RAISE_TIME;
//                    printf("Time: %7.2f  %s nozzle being raised\n", sim_time, nozzle_name[nozzle]);
//                }
//                else
//                {
//                    printf("Time: %7.2f  Bad RAISE_NOZZLE command: nozzle out of range\n", sim_time);
//                }
//            }
//            else if (new_instruction == APPLY_VACUUM)
//            {
//                nozzle = pnp -> instruction_argument_3;
//                if (nozzle >= LEFT_NOZZLE && nozzle <= RIGHT_NOZZLE)
//                {
//                    pnp -> ready_for_next_instruction = FALSE;
//                    pnp -> instruction_to_execute = NO_INSTRUCTION;
//                    instruction_being_executed = APPLY_VACUUM;
//                    instruction_finish_time = sim_time + VACUUM_APPLY_TIME;
//                    printf("Time: %7.2f  %s nozzle is about to apply vacuum\n", sim_time, nozzle_name[nozzle]);
//                }
//                else
//                {
//                    printf("Time: %7.2f  Bad APPLY_VACUUM command: nozzle out of range\n", sim_time);
//                }
//            }
//            else if (new_instruction == RELEASE_VACUUM)
//            {
//                nozzle = pnp -> instruction_argument_3;
//                if (nozzle >= LEFT_NOZZLE && nozzle <= RIGHT_NOZZLE)
//                {
//                    pnp -> ready_for_next_instruction = FALSE;
//                    pnp -> instruction_to_execute = NO_INSTRUCTION;
//                    instruction_being_executed = RELEASE_VACUUM;
//                    instruction_finish_time = sim_time + VACUUM_RELEASE_TIME;
//                    printf("Time: %7.2f  %s nozzle is about to release vacuum\n", sim_time, nozzle_name[nozzle]);
//                 }
//                else
//                {
//                    printf("Time: %7.2f  Bad RELEASE_VACUUM command: nozzle out of range\n", sim_time);
//                }
//            }
//            else if (new_instruction == TAKE_PHOTO)
//            {
//                photo_direction = pnp -> instruction_argument_3;
//                if (photo_direction == PHOTO_LOOKUP || photo_direction == PHOTO_LOOKDOWN)
//                {
//                    pnp -> ready_for_next_instruction = FALSE;
//                    pnp -> instruction_to_execute = NO_INSTRUCTION;
//                    instruction_being_executed = TAKE_PHOTO;
//                    instruction_finish_time = sim_time + PHOTO_TAKE_TIME;
//                    if (photo_direction == PHOTO_LOOKUP)
//                    {
//                        printf("Time: %7.2f  Photo about to be taken by lookup camera\n", sim_time);
//                    }
//                    else
//                    {
//                        printf("Time: %7.2f  Photo about to be taken by lookdown camera\n", sim_time);
//                    }
//                }
//                else
//                {
//                    printf("Time: %7.2f  Bad TAKE_PHOTO command: specified camera is not Lookup or Lookdown\n", sim_time);
//                }
//            }
//            else if (new_instruction == AMEND_HEAD_POSITION)
//            {
//                controller_del_x = pnp -> instruction_argument_1;
//                controller_del_y = pnp -> instruction_argument_2;
//                if (nozzle_down[LEFT_NOZZLE] == FALSE && nozzle_down[CENTRE_NOZZLE] == FALSE && nozzle_down[RIGHT_NOZZLE] == FALSE)
//                {
//                    if (x + controller_del_x >= MIN_X && x + controller_del_x <= MAX_X && y + controller_del_y >= MIN_Y && y + controller_del_y <= MAX_Y)
//                    {
//                        pnp -> ready_for_next_instruction = FALSE;
//                        pnp -> instruction_to_execute = NO_INSTRUCTION;
//                        instruction_being_executed = AMEND_HEAD_POSITION;
//                        instruction_finish_time = sim_time + (double)sqrt(pow((controller_del_x), 2) + pow((controller_del_y), 2)) / HEAD_FULL_SPEED;
//                        printf("Time: %7.2f  Head moving from (%.2f, %.2f) to (%.2f, %.2f)\n", sim_time, x, y, x + controller_del_x, y + controller_del_y);
//                    }
//                    else
//                    {
//                        printf("Time: %7.2f  Bad AMEND_HEAD_POSITION command: destination out of range\n", sim_time);
//                    }
//                }
//                else
//                {
//                    printf("Time: %7.2f  Bad AMEND_HEAD_POSITION command: one or more nozzles down\n", sim_time);
//                }
//            }
//        }
//        /*
//         * If there is an instruction currently being executed, this code checks whether the
//         * instruction has finished based upon the previously calculated instruction finish time.
//         * If so, variables are updated based upon the type of the instruction that was executed
//         * (e.g. x and y for MOVE_HEAD).
//         *
//         * It then signals that there is currently no instruction being executed back to the controller
//         * so that the controller can issue its next instruction if required.
//         */
//        else if (sim_time >= instruction_finish_time)
//        {
//            int feeder;
//            switch(instruction_being_executed)
//            {
//                case MOVE_HEAD:
//                    x = x_target;
//                    y = y_target;
//                    printf("Time: %7.2f  Head arrived at nominal location (%.2f, %.2f)\n", sim_time, x, y);
//                    break;
//
//                case ROTATE_NOZZLE:
//                    theta_actual[nozzle] = theta_actual[nozzle] + controller_theta;
//                    printf("Time: %7.2f  %s nozzle finished rotating by %.2f degrees, effective rotation including misalignment theta_error=%.2f degrees is %.2f degrees\n",
//                            sim_time, nozzle_name[nozzle], controller_theta, theta_pick_error[nozzle], theta_actual[nozzle]);
//                    break;
//
//                case LOWER_NOZZLE:
//                    nozzle_down[nozzle] = TRUE;
//                    printf("Time: %7.2f  %s nozzle lowered\n", sim_time, nozzle_name[nozzle]);
//                    /* code for when part is being picked up from tape feeder */
//                    feeder = getTapeFeederNumberAtLocation(x + (nozzle - CENTRE_NOZZLE) * NOZZLE_X_SEPARATION,y);
//                    if (nozzle_vacuum[nozzle] == TRUE
//                        && nozzle_picked_part[nozzle] == NO_PICKED_PART
//                        && feeder != NO_TAPE_FEEDER_AT_THIS_LOCATION)
//                    {
//                        nozzle_picked_part[nozzle] = feeder;
//                        printf("Time: %7.2f  %s nozzle has picked up part from feeder %d\n", sim_time, nozzle_name[nozzle], feeder);
//                    }
//                    else if (nozzle_vacuum[nozzle] == TRUE
//                            && nozzle_picked_part[nozzle] == NO_PICKED_PART)
//                    {
//                        printf("Time: %7.2f  No tape feeder underneath nozzle %s when vacuum applied so no part picked up\n", sim_time, nozzle_name[nozzle]);
//                    }
//                    break;
//
//                case RAISE_NOZZLE:
//                    nozzle_down[nozzle] = FALSE;
//                    printf("Time: %7.2f  %s nozzle raised\n", sim_time, nozzle_name[nozzle]);
//                    break;
//
//                case APPLY_VACUUM:
//                    nozzle_vacuum[nozzle] = TRUE;
//                    printf("Time: %7.2f  %s nozzle now has vacuum applied\n", sim_time, nozzle_name[nozzle]);
//                    /* code for when part is being picked up from tape feeder */
//                    feeder = getTapeFeederNumberAtLocation(x + (nozzle - CENTRE_NOZZLE) * NOZZLE_X_SEPARATION,y);
//                    if (nozzle_down[nozzle] == TRUE
//                        && nozzle_picked_part[nozzle] == NO_PICKED_PART
//                        && feeder != NO_TAPE_FEEDER_AT_THIS_LOCATION)
//                    {
//                        nozzle_picked_part[nozzle] = feeder;
//                        printf("Time: %7.2f  %s nozzle has picked up part from feeder %d\n", sim_time, nozzle_name[nozzle], feeder);
//                    }
//                    else if (nozzle_down[nozzle] == TRUE && nozzle_picked_part[nozzle] == NO_PICKED_PART)
//                    {
//                        printf("Time: %7.2f  No tape feeder underneath nozzle %s when vacuum applied so no part picked up\n", sim_time, nozzle_name[nozzle]);
//                    }
//                    break;
//
//                case RELEASE_VACUUM:
//                    nozzle_vacuum[nozzle] = FALSE;
//                    printf("Time: %7.2f  %s nozzle now has vacuum released\n", sim_time, nozzle_name[nozzle]);
//                    /* code for when part is being placed on PCB */
//                    if (nozzle_down[nozzle] == TRUE
//                        && nozzle_picked_part[nozzle] != NO_PICKED_PART
//                        && x >= 0.0 && y >= 0.0)
//                    {
//                        printf("Time: %7.2f  %s nozzle has placed part from feeder %d at (%.2f, %.2f) with rotation %.2f degrees\n",
//                               sim_time, nozzle_name[nozzle], nozzle_picked_part[nozzle], x, y, theta_actual[nozzle]);
//                        placedPart[number_of_placed_parts].x_actual = x;
//                        placedPart[number_of_placed_parts].y_actual = y;
//                        placedPart[number_of_placed_parts].theta_actual = theta_actual[nozzle];
//                        placedPart[number_of_placed_parts].feeder = nozzle_picked_part[nozzle];
//                        number_of_placed_parts++;
//                        printf("\nSummary of placed parts so far:\n");
//                        for (int i = 0; i < number_of_placed_parts; i++)
//                        {
//                            printf("Part %d from feeder %d placed at (%.2f, %.2f) with rotation %.2f degrees\n", i,
//                                   placedPart[i].feeder, placedPart[i].x_actual, placedPart[i].y_actual, placedPart[i].theta_actual);
//                        }
//                        printf("\n");
//                        nozzle_picked_part[nozzle] = NO_PICKED_PART;
//
//                        /* reset pick and preplace alignment error values after part placed */
//                        x_preplace_error = 0.0;
//                        y_preplace_error = 0.0;
//                        theta_pick_error[nozzle] = 0.0;
//                        pnp -> x_preplace_error = x_preplace_error;
//                        pnp -> y_preplace_error = y_preplace_error;
//                        pnp -> theta_pick_error[nozzle] = theta_pick_error[nozzle];
//                        theta_actual[nozzle] = 0.0;
//
//                    }
//                    /* code for when part is dropped from a height */
//                    else if (nozzle_down[nozzle] == FALSE
//                             && nozzle_picked_part[nozzle] != NO_PICKED_PART)
//                    {
//                        printf("Time: %7.2f  %s nozzle has DROPPED part from feeder %d at (%.2f, %.2f)\n", sim_time, nozzle_name[nozzle], nozzle_picked_part[nozzle], x, y);
//                        number_of_dropped_parts++;
//                        nozzle_picked_part[nozzle] = NO_PICKED_PART;
//                    }
//                    break;
//
//                case TAKE_PHOTO:
//                    /* code for when lookup camera is used to take photos to discover pick misalignment */
//                    if (photo_direction == PHOTO_LOOKUP && x == LOOKUP_CAMERA_X && y == LOOKUP_CAMERA_Y)
//                    {
//                        printf("Time: %7.2f  Photo taken by lookup camera\n", sim_time);
//                        for (int i = 0; i < NUMBER_OF_NOZZLES; i++)
//                        {
//                            if (nozzle_picked_part[i] != NO_PICKED_PART)
//                            {
//                                theta_pick_error[i] = MAX_THETA_PICK_MISALIGNMENT * (double)rand()/RAND_MAX - MAX_THETA_PICK_MISALIGNMENT / 2;
//                                theta_actual[i] = theta_pick_error[i];
//
//                                printf("Time: %7.2f  Picked part on %s nozzle has misalignment theta_error=%.2f degrees\n", sim_time, nozzle_name[i], theta_pick_error[i]);
//
//                                pnp -> theta_pick_error[i] = theta_pick_error[i];
//                            }
//                        }
//                     }
//                     else if (photo_direction == PHOTO_LOOKDOWN && x >= 0.0 && y >= 0.0)
//                     {
//                        printf("Time: %7.2f  Photo taken by lookdown camera\n", sim_time);
//
//                        x_preplace_error = MAX_X_PREPLACE_MISALIGNMENT * (double)rand()/RAND_MAX - MAX_X_PREPLACE_MISALIGNMENT / 2;
//                        y_preplace_error = MAX_Y_PREPLACE_MISALIGNMENT * (double)rand()/RAND_MAX - MAX_Y_PREPLACE_MISALIGNMENT / 2;
//
//                        printf("Time: %7.2f  Head has preplace misalignment x_error=%.2f y_error=%.2f\n", sim_time, x_preplace_error, y_preplace_error);
//
//                        x = x + x_preplace_error;
//                        y = y + y_preplace_error;
//
//                        pnp -> x_preplace_error = x_preplace_error;
//                        pnp -> y_preplace_error = y_preplace_error;
//
//                     }
//                     break;
//
//                case AMEND_HEAD_POSITION:
//                    x = x + controller_del_x;
//                    y = y + controller_del_y;
//                    printf("Time: %7.2f  Head position amended to (%.2f, %.2f)\n", sim_time, x, y);
//                    break;
//            }
//
//            /* update shared memory for instruction related variables */
//            instruction_being_executed = NO_INSTRUCTION;
//            pnp -> ready_for_next_instruction = TRUE;
//        }
//
//        sleepMilliseconds((long) 1000 / POLL_LOOP_RATE);
//        sim_time += (double) 1 / POLL_LOOP_RATE;
//
//        /* update shared memory for simulation time (since this must always be updated every poll cycle) */
//        pnp -> sim_time = sim_time;
//    }
//
//    /* unmap memory and close file descriptor before exit */
//    sleep(1);
//    resetPnP(pnp, 0.0);
//    munmap(pnp, sizeof(PnP));
//    close(fd);
//
//    exit(20);
}
