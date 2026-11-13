#include "types.h"
#include "private/controller_input.h"

extern unsigned char data_020a0e44;
extern u16 data_020a0e48[4];
extern u16 data_020a0e50[4];
extern PadData data_020a0e58[4];

extern u16 _Z13func_0203dae4i(int idx);

void func_0203bc7c(void) {
    PadData *p = data_020a0e58;
    int i;
    data_020a0e44 = 0;
    for (i = 0; i < 4; i++) {
        u16 keys = _Z13func_0203dae4i(i);
        u16 old;
        int changed;
        if ((keys & 0x30) == 0x30)
            keys &= ~0x30;
        if ((keys & 0xc0) == 0xc0)
            keys &= ~0xc0;
        /* L+R+START+SELECT: the soft reset combo (func_0202345c.cpp). */
        if (keys == 0x30c)
            data_020a0e44 = 1;
        old = data_020a0e50[i];
        changed = keys ^ old;
        p->pressed = keys & changed;
        data_020a0e48[i] = old & changed;
        data_020a0e50[i] = keys;
        p->held = keys;
        p++;
    }
}
