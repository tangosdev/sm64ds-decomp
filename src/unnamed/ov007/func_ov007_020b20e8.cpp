//cpp
#include "types.h"
#include "Ov007Work.h"
#include "G3i.h"

extern "C" {

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
    char pad_74[0x40];
    int scaleW; /* 0xb4: PerspectiveW_'s scale (func_ov007_020b2160) */
    char pad_b8[0x1c];
    unsigned short fovy; /* 0xd4 */
} Ov007Camera;

typedef char Ov007Camera_fovy_must_be_at_0xd4[(int)&((Ov007Camera *)0)->fovy == 0xd4 ? 1 : -1];

extern char* data_ov007_0210342c;
extern void func_ov007_020c0128(char* c);
extern void func_ov007_020bffb8(char* c);
extern void func_ov007_020c0078(char *c);

/* The extern keeps the tree's char * spelling; the typed view is reached
 * through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* Resets the second camera and frames it: distance 8, fovy 0x4da, scaleW
 * 0x12bb, then orbits the eye and rebuilds its view matrix. */
void func_ov007_020b20e8(void){
    Ov007Camera *cam = WORK->cameras[1];
    func_ov007_020c0128((char *)cam);
    cam->distance = 0x8000;
    cam->fovy = 0x4da;
    cam->scaleW = 0x12bb;
    func_ov007_020bffb8((char *)cam);
    G3i::LookAt_(&cam->eye, &cam->up, &cam->target, true, (Matrix4x3 *)cam->viewMatrix);
    func_ov007_020c0078((char *)cam);
}
}
