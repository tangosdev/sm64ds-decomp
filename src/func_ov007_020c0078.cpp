//cpp
#include "nitro/gx/g3_util.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_trig.h"

extern "C" {

/* func_ov007_020c0078 at 0x020c0078 (ov007)
 *
 * Ov007Camera perspective setup: writes the viewport register (0x4000580)
 * from the camera's screen rectangle, then calls G3i_PerspectiveW_ with a
 * sine-table pair for fovy, aspect >> 20, nearZ and farZ into projMatrix,
 * and finally clears 0x400044c.
 *
 * Matched byte-for-byte with mwccarm 2004/b56.
 */
void func_ov007_020c0078(Ov007Camera *cam) {
    int a;
    *(int *)0x4000580 = cam->viewportMinX | (cam->viewportMinY << 8) | (cam->viewportMaxX << 16) |
                        (cam->viewportMaxY << 24);

    a = (int)cam->fovy >> 4;
    G3i_PerspectiveW_(FX_SinIdx(a), FX_CosIdx(a), (int)(cam->aspect >> 0x14), cam->nearZ, cam->farZ,
                      0x1000, 1, (Matrix4x3 *)cam->projMatrix);

    *(int *)0x400044c = 0;
}
}
