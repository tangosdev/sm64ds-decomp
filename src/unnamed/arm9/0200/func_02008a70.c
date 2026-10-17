#include "PlayerInput.h"
extern int data_0209f4a4;

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    char _pad[0x19c];
    /* func_02008b08 eases it toward the active player's camera-input entry
       (data_0209f4a0); this function and func_02008abc scale that entry's
       two axes by it before func_020095e4 turns the camera. */
    short mInputScale; /* 0x19c */
} dCamera_c;

short func_02008a70(dCamera_c *self)
{
    unsigned char idx = *(unsigned char *)&gActivePlayerSlot;
    short tbl = *(short *)((char *)&data_0209f4a4 + idx * 0x18);
    short val = self->mInputScale;
    long long prod = (long long)tbl * (long long)val;
    prod += 0x800LL;
    int tmp = (int)(prod >> 12);
    return (short)tmp;
}
