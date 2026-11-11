#include "types.h"
#include "Ov007StrokePath.h"
extern void MIi_CpuClear32(int val, int *dst, int len);
extern void MIi_CpuClear16(u16 val, char *dst, int nbytes);

/* Clears the path for reuse: no points, and every per-point table zeroed
 * for its capacity -- the planar coordinates or the 12-byte point storage,
 * the segment angles (planar only) and lengths, and the progress thresholds
 * when the path has them. */
void func_ov007_020c28ac(Ov007StrokePath *c) {
    c->pointCount = 0;
    switch (c->representation) {
    case OV007_PATH_PLANAR: {
        volatile int a = 0;
        MIi_CpuClear32(a, c->xCoords, c->capacity << 2);
        {
            volatile int b = 0;
            MIi_CpuClear32(b, c->yCoords, c->capacity << 2);
        }
        break;
    }
    case 1: {
        volatile int a = 0;
        MIi_CpuClear32(a, (int *)c->pointStorage, c->capacity * 0xc);
        break;
    }
    }
    switch (c->representation) {
    case OV007_PATH_PLANAR: {
        volatile u16 a = 0;
        MIi_CpuClear16(a, (char *)c->segmentAngles, c->capacity << 1);
    }
    case 1:
    default: {
        volatile int b = 0;
        MIi_CpuClear32(b, c->segmentLengths, c->capacity << 2);
    }
    }
    if (c->flags & OV007_PATH_HAS_PROGRESS) {
        volatile int a = 0;
        MIi_CpuClear32(a, c->progressThresholds, c->capacity << 2);
    }
}
