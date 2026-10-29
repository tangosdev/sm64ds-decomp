//cpp
#include "Ov007Work.h"
#include "G3i.h"

extern "C" {

typedef int Fix12;

/* The overlay's camera (func_ov007_020c023c); names as in
 * func_ov007_020c0128, only the slots this file reads. */
typedef struct Ov007Camera {
    char pad_00[8];
    Vector3 eye;    /* 0x08 */
    Vector3 target; /* 0x14 */
    Vector3 up;     /* 0x20 */
    char pad_2c[0x18];
    int viewMatrix[12]; /* 0x44: Matrix4x3 */
    int projMatrix[16]; /* 0x74: Matrix4x4 */
    int scaleW;         /* 0xb4 */
    char pad_b8[4];
    long long aspect; /* 0xbc: PerspectiveW_ takes it >> 0x14 */
    int nearZ;        /* 0xc4 */
    int farZ;         /* 0xc8 */
    char pad_cc[8];
    /* 0xd4: a u16 angle; this file indexes the sine table with it >> 4 */
    unsigned short fovy;
} Ov007Camera;

extern void func_ov007_020bff38(char *c);
extern void func_ov007_020bffb8(char *c);

extern char *data_ov007_0210342c;
extern short data_02082214[];

/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

void func_ov007_020b2160(int mode) {
    int a;

    switch (mode) {
    case 1:
        a = (int)WORK->cameras[1]->fovy >> 4;
        G3i::PerspectiveW_(data_02082214[a * 2], data_02082214[a * 2 + 1],
                           (int)(WORK->cameras[1]->aspect >> 0x14), WORK->cameras[1]->nearZ,
                           WORK->cameras[1]->farZ, WORK->cameras[1]->scaleW, 1,
                           (Matrix4x3 *)WORK->cameras[1]->projMatrix);
        G3i::LookAt_(&WORK->cameras[1]->eye, &WORK->cameras[1]->up, &WORK->cameras[1]->target, true,
                     (Matrix4x3 *)WORK->cameras[1]->viewMatrix);
        break;
    case 0:
        a = (int)WORK->cameras[0]->fovy >> 4;
        G3i::PerspectiveW_(data_02082214[a * 2], data_02082214[a * 2 + 1],
                           (int)(WORK->cameras[0]->aspect >> 0x14), WORK->cameras[0]->nearZ,
                           WORK->cameras[0]->farZ, 0x1000, 1, (Matrix4x3 *)0);
        func_ov007_020bff38((char *)WORK->cameras[0]);
        func_ov007_020bffb8((char *)WORK->cameras[0]);
        G3i::LookAt_(&WORK->cameras[0]->eye, &WORK->cameras[0]->up, &WORK->cameras[0]->target, true,
                     (Matrix4x3 *)WORK->cameras[0]->viewMatrix);
        break;
    }
}
}
