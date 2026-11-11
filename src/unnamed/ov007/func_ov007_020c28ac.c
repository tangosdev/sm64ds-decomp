#include "types.h"
#include "Ov007StrokePath.h"
#include "nitro/mi/memory.h"

/* Clears the path for reuse: no points, and every per-point table zeroed
 * for its capacity -- the planar coordinates or the 12-byte point storage,
 * the segment angles (planar only) and lengths, and the progress thresholds
 * when the path has them. */
void func_ov007_020c28ac(Ov007StrokePath *c) {
    c->pointCount = 0;
    switch (c->representation) {
    case OV007_PATH_PLANAR: {
        volatile int a = 0;
        MI_CpuFill32(c->xCoords, a, c->capacity << 2);
        {
            volatile int b = 0;
            MI_CpuFill32(c->yCoords, b, c->capacity << 2);
        }
        break;
    }
    case 1: {
        volatile int a = 0;
        MI_CpuFill32(c->pointStorage, a, c->capacity * 0xc);
        break;
    }
    }
    switch (c->representation) {
    case OV007_PATH_PLANAR: {
        volatile u16 a = 0;
        MI_CpuFill16(c->segmentAngles, a, c->capacity << 1);
    }
    case 1:
    default: {
        volatile int b = 0;
        MI_CpuFill32(c->segmentLengths, b, c->capacity << 2);
    }
    }
    if (c->flags & OV007_PATH_HAS_PROGRESS) {
        volatile int a = 0;
        MI_CpuFill32(c->progressThresholds, a, c->capacity << 2);
    }
}
