#include "types.h"
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off
typedef struct InputPair
{
    u16 cur;
    u16 pressed;
} InputPair;

extern unsigned char data_020a0e44;
extern u16 data_020a0e48[4];
extern u16 data_020a0e50[4];
extern InputPair data_020a0e58[4];

extern u16 func_0203dae4(int idx);

void func_0203bc7c(void)
{
    InputPair *p = data_020a0e58;
    int i;
    data_020a0e44 = 0;
    for (i = 0; i < 4; i++)
    {
        u16 keys = func_0203dae4(i);
        u16 old;
        int changed;
        if ((keys & 0x30) == 0x30)
            keys &= ~0x30;
        if ((keys & 0xc0) == 0xc0)
            keys &= ~0xc0;
        if (keys == 0x30c)
            data_020a0e44 = 1;
        old = data_020a0e50[i];
        changed = keys ^ old;
        p->pressed = keys & changed;
        data_020a0e48[i] = old & changed;
        data_020a0e50[i] = keys;
        p->cur = keys;
        p++;
    }
}
