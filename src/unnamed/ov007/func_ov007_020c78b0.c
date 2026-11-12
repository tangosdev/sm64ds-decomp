#include "Ov007Spring.h"
#include "nitro/mi/memory.h"

/* Clears a point list func_ov007_020c798c has just allocated. */
void func_ov007_020c78b0(Ov007PointList *list) {
    volatile int li = 0;
    MI_CpuFill32(list, li, sizeof(Ov007PointList));
}
