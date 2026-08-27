#include "types.h"
extern u8 data_0209f250;
extern void *data_0209f394[];
extern int func_020bd828(void *p);

int func_02029408(void)
{
    return func_020bd828(data_0209f394[data_0209f250]);
}
