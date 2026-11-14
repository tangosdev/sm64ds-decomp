//cpp
#include "nitro/gx/g3_util.h"
#include "nitro/gx/g3imm_cmd.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_trig.h"

/* func_ov007_020c0078 at 0x020c0078 (ov007)
 *
 * Ov007Camera perspective setup: sets the viewport from the camera's screen
 * rectangle, then calls G3i_PerspectiveW_ with a sine-table pair for fovy,
 * aspect >> 20, nearZ and farZ into projMatrix, and finally stores the
 * current matrix to stack slot 0.
 *
 * Matched byte-for-byte with mwccarm 2004/b56.
 */
void func_ov007_020c0078(Ov007Camera *cam) {
    int a;
    G3_ViewPort(cam->viewportMinX, cam->viewportMinY, cam->viewportMaxX, cam->viewportMaxY);

    a = cam->fovy >> 4;
    G3i_PerspectiveW_(FX_SinIdx(a), FX_CosIdx(a), (cam->aspect >> 0x14), cam->nearZ, cam->farZ,
                      0x1000, 1, (Matrix4x3 *)cam->projMatrix);

    G3_StoreMtx(0);
}
