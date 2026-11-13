//cpp
#include "types.h"
#include "Ov007Work.h"
#include "nitro/gx/g3_util.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_cp.h"

extern "C" void func_ov007_020c0128(Ov007Camera *cam);
extern "C" void func_ov007_020bffb8(Ov007Camera *cam);
extern void func_ov007_020c0078(Ov007Camera *cam);

/* preset: 1 (func_ov007_020b10dc), 4 (func_ov007_020b0a20), 2 and 3. */
void func_ov007_020b1fa4(int preset) {
    Ov007Camera *cam = WORK->cameras[0];
    func_ov007_020c0128(cam);
    switch (preset) {
    case 1:
        switch (WORK->figureIndex) {
        case 0:
            cam->target.y = -0x3e8;
            cam->distance = 0x1e000;
            cam->fovy = (short)FX_DivS32(0xf0000, 0x168);
            break;
        case 1:
            cam->target.y = -0x7d0;
            cam->distance = 0x28000;
            cam->fovy = (short)FX_DivS32(0x120000, 0x168);
            break;
        }
        break;
    case 2:
        cam->distance = 0xa000;
        break;
    case 3:
        cam->distance = 0x5000;
        cam->fovy = (short)FX_DivS32(0x140000, 0x168);
        break;
    case 4:
        cam->target.y = -0x3e8;
        cam->distance = 0x1e000;
        cam->fovy = (short)FX_DivS32(0xf0000, 0x168);
        break;
    }
    func_ov007_020bffb8(cam);
    G3i_LookAt_(&cam->eye, &cam->up, &cam->target, true, &cam->viewMatrix);
    func_ov007_020c0078(cam);
    WORK->cameraPreset = preset;
}
