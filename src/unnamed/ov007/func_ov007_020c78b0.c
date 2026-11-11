#include "Ov007Spring.h"

extern void MIi_CpuClear32(int val, void *dst, int size);

/* Clears a point list func_ov007_020c798c has just allocated. */
void func_ov007_020c78b0(Ov007PointList *list) {
    volatile int li = 0;
    MIi_CpuClear32(li, list, sizeof(Ov007PointList));
}
