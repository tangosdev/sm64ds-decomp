#include "Ov007StrokePath.h"

extern int func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c29b4(char* c);
extern void func_ov007_020c28ac(void* c);

/* Allocates a stroke path of representation a with room for b points:
 * planar paths get their X/Y coordinate and segment-angle arrays, the
 * alternate one its 12-byte point storage; every path gets segment lengths,
 * and progress thresholds when flags has OV007_PATH_HAS_PROGRESS. */
int func_ov007_020c2c68(int a, int b, int cc)
{
    char* p = (char*)func_ov007_020c3df4(0, 0x30);
    Ov007StrokePath *path = (Ov007StrokePath *)p;
    func_ov007_020c29b4(p);
    path->representation = a;
    path->flags = cc;
    path->capacity = b;
    switch (a) {
    case OV007_PATH_PLANAR:
        path->xCoords = (int *)func_ov007_020c3df4(0, b << 2);
        path->yCoords = (int *)func_ov007_020c3df4(0, b << 2);
        path->segmentAngles = (u16 *)func_ov007_020c3df4(0, b << 1);
        break;
    case 1:
        path->pointStorage = (void *)func_ov007_020c3df4(0, b * 0xc);
        break;
    }
    path->segmentLengths = (int *)func_ov007_020c3df4(0, b << 2);
    if (path->flags & OV007_PATH_HAS_PROGRESS) {
        path->progressThresholds = (int *)func_ov007_020c3df4(0, b << 2);
    }
    func_ov007_020c28ac(p);
    return (int)p;
}
