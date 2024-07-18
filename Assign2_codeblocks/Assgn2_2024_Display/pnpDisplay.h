#include <fcntl.h>
#include <sys/mman.h>

#define MEMORY_MAPPED_FILE "pnp_shared_file"
#define NUMBER_OF_NOZZLES 3

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
