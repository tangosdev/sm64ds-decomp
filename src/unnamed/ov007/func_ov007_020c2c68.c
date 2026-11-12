#include "Ov007StrokePath.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c29b4(Ov007StrokePath *path);
extern void func_ov007_020c28ac(void* c);

/* Allocates a stroke path of representation a with room for b points:
 * planar paths get their X/Y coordinate and segment-angle arrays, the
 * alternate one its 12-byte point storage; every path gets segment lengths,
 * and progress thresholds when flags has OV007_PATH_HAS_PROGRESS. */
Ov007StrokePath *func_ov007_020c2c68(int a, int b, int cc) {
    Ov007StrokePath *path = func_ov007_020c3df4(0, 0x30);
    func_ov007_020c29b4(path);
    path->representation = a;
    path->flags = cc;
    path->capacity = b;
    switch (a) {
    case OV007_PATH_PLANAR:
        path->xCoords = func_ov007_020c3df4(0, b << 2);
        path->yCoords = func_ov007_020c3df4(0, b << 2);
        path->segmentAngles = func_ov007_020c3df4(0, b << 1);
        break;
    case 1:
        path->pointStorage = func_ov007_020c3df4(0, b * 0xc);
        break;
    }
    path->segmentLengths = func_ov007_020c3df4(0, b << 2);
    if (path->flags & OV007_PATH_HAS_PROGRESS) {
        path->progressThresholds = func_ov007_020c3df4(0, b << 2);
    }
    func_ov007_020c28ac(path);
    return path;
}
