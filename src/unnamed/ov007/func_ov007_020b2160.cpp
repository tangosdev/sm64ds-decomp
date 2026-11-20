//cpp
#include "Ov007Work.h"
#include "nitro/gx/g3_util.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_trig.h"


extern "C" void func_ov007_020bff38(Ov007Camera *cam);
extern "C" void func_ov007_020bffb8(Ov007Camera *cam);

void func_ov007_020b2160(int mode) {
    int a;

    switch (mode) {
    case 1:
        a = gMenuWork->cameras[1]->fovy >> 4;
        G3i_PerspectiveW_(FX_SinIdx(a), FX_CosIdx(a), (gMenuWork->cameras[1]->aspect >> 20),
                          gMenuWork->cameras[1]->nearZ, gMenuWork->cameras[1]->farZ,
                          gMenuWork->cameras[1]->scaleW, 1,
                          (Matrix4x3 *)gMenuWork->cameras[1]->projMatrix);
        G3i_LookAt_(&gMenuWork->cameras[1]->eye, &gMenuWork->cameras[1]->up,
                    &gMenuWork->cameras[1]->target, true, &gMenuWork->cameras[1]->viewMatrix);
        break;
    case 0:
        a = gMenuWork->cameras[0]->fovy >> 4;
        G3i_PerspectiveW_(FX_SinIdx(a), FX_CosIdx(a), (gMenuWork->cameras[0]->aspect >> 20),
                          gMenuWork->cameras[0]->nearZ, gMenuWork->cameras[0]->farZ, 0x1000, 1,
                          (Matrix4x3 *)0);
        func_ov007_020bff38(gMenuWork->cameras[0]);
        func_ov007_020bffb8(gMenuWork->cameras[0]);
        G3i_LookAt_(&gMenuWork->cameras[0]->eye, &gMenuWork->cameras[0]->up,
                    &gMenuWork->cameras[0]->target, true, &gMenuWork->cameras[0]->viewMatrix);
        break;
    }
}
