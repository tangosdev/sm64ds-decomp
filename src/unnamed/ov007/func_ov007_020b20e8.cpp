//cpp
#include "types.h"
#include "Ov007Work.h"
#include "nitro/gx/g3_util.h"
#include "Ov007Camera.h"

extern "C" void func_ov007_020c0128(Ov007Camera *cam);
extern "C" void func_ov007_020bffb8(Ov007Camera *cam);
extern void func_ov007_020c0078(Ov007Camera *cam);

/* Resets the second camera and frames it: distance 8, fovy 0x4da, scaleW
 * 0x12bb, then orbits the eye and rebuilds its view matrix. */
extern "C" void func_ov007_020b20e8(void){
    Ov007Camera *cam = WORK->cameras[1];
    func_ov007_020c0128(cam);
    cam->distance = 0x8000;
    cam->fovy = 0x4da;
    cam->scaleW = 0x12bb;
    func_ov007_020bffb8(cam);
    G3i_LookAt_(&cam->eye, &cam->up, &cam->target, true, &cam->viewMatrix);
    func_ov007_020c0078(cam);
}
