//cpp
#include "Ov007StrokePath.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

extern int func_ov007_020c2dfc(Ov007StrokePath *path, int idx, int v);
extern void func_ov007_020c24d0(Ov007StrokePath *path, int idx);

/* Straightens a planar path: walks its points and drops each one where the
 * path turns by less than minTurn degrees (func_ov007_020c2dfc). The
 * FX_DivS32 result is unused; the call stays because the ROM makes it. */
void func_ov007_020c281c(Ov007StrokePath *path, int minTurn) {
    int i;
    if (path->pointCount < 3)
        return;
    FX_DivS32(minTurn << 16, 0x168);
    switch (path->representation) {
    case OV007_PATH_PLANAR:
        for (i = 2; i < path->pointCount; i++) {
            if (func_ov007_020c2dfc(path, i, minTurn) != 0) {
                func_ov007_020c24d0(path, i - 1);
            }
        }
        break;
    case 1:
        break;
    }
}
}
