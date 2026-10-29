//cpp
#include "types.h"
#include "Ov007Work.h"
#include "G3i.h"
#include "cstd.h"

extern "C" {

extern int func_ov007_020c0128(void *c);
extern void func_ov007_020bffb8(void *c);
extern void func_ov007_020c0078(void* c);
extern char* data_ov007_0210342c;

/* The 0xd8-byte camera func_ov007_020c023c builds; names as in
 * func_ov007_020c0128, only the slots this file touches. */
typedef struct Ov007Camera {
    char pad_00[8];
    Vector3 eye;    /* 0x08 */
    Vector3 target; /* 0x14 */
    Vector3 up;     /* 0x20 */
    int distance;   /* 0x2c */
    char pad_30[0x14];
    int viewMatrix[12]; /* 0x44: Matrix4x3 */
    char pad_74[0x60];
    short fovy; /* 0xd4 */
} Ov007Camera;

#define WORK ((Ov007Work *)data_ov007_0210342c)

/* preset: 1 (func_ov007_020b10dc), 4 (func_ov007_020b0a20), 2 and 3. */
void func_ov007_020b1fa4(int preset) {
    Ov007Camera *cam = WORK->cameras[0];
    func_ov007_020c0128(cam);
    switch (preset) {
    case 1:
        switch (*(int*)((char*)data_ov007_0210342c + 0xec))
        {
        case 0:
            cam->target.y = -0x3e8;
            cam->distance = 0x1e000;
            cam->fovy = (short)cstd::div(0xf0000, 0x168);
            break;
        case 1:
            cam->target.y = -0x7d0;
            cam->distance = 0x28000;
            cam->fovy = (short)cstd::div(0x120000, 0x168);
            break;
        }
        break;
    case 2:
        cam->distance = 0xa000;
        break;
    case 3:
        cam->distance = 0x5000;
        cam->fovy = (short)cstd::div(0x140000, 0x168);
        break;
    case 4:
        cam->target.y = -0x3e8;
        cam->distance = 0x1e000;
        cam->fovy = (short)cstd::div(0xf0000, 0x168);
        break;
    }
    func_ov007_020bffb8(cam);
    G3i::LookAt_(&cam->eye, &cam->up, &cam->target, true, (Matrix4x3 *)cam->viewMatrix);
    func_ov007_020c0078(cam);
    WORK->cameraPreset = preset;
}
}
