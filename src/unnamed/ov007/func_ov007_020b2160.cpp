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
        a = sMenuWork->cameras[1]->fovy >> 4;
        G3i_PerspectiveW_(FX_SinIdx(a), FX_CosIdx(a), (sMenuWork->cameras[1]->aspect >> 20),
                          sMenuWork->cameras[1]->nearZ, sMenuWork->cameras[1]->farZ,
                          sMenuWork->cameras[1]->scaleW, 1,
                          (Matrix4x3 *)sMenuWork->cameras[1]->projMatrix);
        G3i_LookAt_(&sMenuWork->cameras[1]->eye, &sMenuWork->cameras[1]->up,
                    &sMenuWork->cameras[1]->target, true, &sMenuWork->cameras[1]->viewMatrix);
        break;
    case 0:
        a = sMenuWork->cameras[0]->fovy >> 4;
        G3i_PerspectiveW_(FX_SinIdx(a), FX_CosIdx(a), (sMenuWork->cameras[0]->aspect >> 20),
                          sMenuWork->cameras[0]->nearZ, sMenuWork->cameras[0]->farZ, 0x1000, 1,
                          (Matrix4x3 *)0);
        func_ov007_020bff38(sMenuWork->cameras[0]);
        func_ov007_020bffb8(sMenuWork->cameras[0]);
        G3i_LookAt_(&sMenuWork->cameras[0]->eye, &sMenuWork->cameras[0]->up,
                    &sMenuWork->cameras[0]->target, true, &sMenuWork->cameras[0]->viewMatrix);
        break;
    }
}
