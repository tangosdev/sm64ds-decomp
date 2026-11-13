//cpp
#include "types.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"
extern "C" void MTX_Identity44_(void *p);

/* Resets a camera: eye at z 1.0 looking at the origin, y up, a 30-degree
 * fovy, 4:3 aspect, near 1.0 and far 400.0, the whole 256x192 screen as the
 * viewport and identity view and projection matrices. */
void func_ov007_020c0128(Ov007Camera *cam) {
    cam->eye.y = 0;
    cam->eye.x = cam->eye.y;
    cam->eye.z = 0x1000;
    cam->target.z = 0;
    cam->target.y = cam->target.z;
    cam->target.x = cam->target.y;
    cam->distance = 0;
    cam->half34 = 0;
    cam->azimuth = cam->half34;
    cam->elevation = cam->azimuth;
    cam->up.z = 0;
    cam->up.x = cam->up.z;
    cam->up.y = 0x1000;
    cam->nearZ = 0x1000;
    cam->farZ = 0x190000;
    cam->fovy = (short)FX_DivS32(0x1e0000, 0x168);
    long long l = FX_DivFx64c(0x4000, 0x3000);
    cam->aspect = l;
    cam->viewportMinX = 0;
    cam->viewportMaxX = 0xff;
    cam->viewportMinY = 0;
    cam->viewportMaxY = 0xbf;
    cam->wordCC = 0x1000;
    cam->wordD0 = 0xc00;
    MTX_Identity43_(&cam->viewMatrix);
    MTX_Identity44_(cam->projMatrix);
    cam->scaleW = 0x1000;
    cam->word00 = 0;
    cam->tweening = 0;
}
