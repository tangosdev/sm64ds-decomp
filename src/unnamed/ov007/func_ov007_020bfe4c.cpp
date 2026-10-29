//cpp
#include "nitro/fx/fx.h"
#include "cstd.h"

extern "C" {

typedef struct { int x, y, z; } Vector3;
typedef struct { int m[12]; } Matrix4x3;

extern void InvMat4x3(void* dst, void* out);
extern void MulVec3Mat4x3(void* v, void* m, void* res);
extern short data_02082214[];

/* The 0xd8-byte camera func_ov007_020c023c allocates; names as in
 * func_ov007_020c0128, only the slots this file reads. */
typedef struct Ov007Camera {
    char pad_00[0x44];
    int viewMatrix[12]; /* 0x44: Matrix4x3 */
    char pad_74[0x60];
    /* 0xd4: a u16 angle; indexes the sine table >> 4, as in func_ov007_020b2160 */
    unsigned short fovy;
} Ov007Camera;

typedef char Ov007Camera_fovy_must_be_at_0xd4[(int)&((Ov007Camera *)0)->fovy == 0xd4 ? 1 : -1];

/* Unprojects the screen point (a, b), in 20.12 pixels, at depth c through
 * the camera's fovy and inverse view matrix. thiz keeps the char * spelling
 * of the definition; `cam` is the typed view. */

void func_ov007_020bfe4c(char* thiz, int a, int b, int c, Vector3* out)
{
    Ov007Camera *cam = (Ov007Camera *)thiz;
    Vector3 v;
    Matrix4x3 inv;
    int ang, i, cosv, sinv, ac, f1;
    v.x = a - 0x80000;
    v.y = -(b - 0x60000);
    v.z = c;
    ang = (int)cam->fovy >> 4;
    i = ang * 2;
    sinv = data_02082214[i + 1];
    cosv = data_02082214[i];
    ac = c < 0 ? -c : c;
    f1 = cstd::fdiv(FX_MUL(cosv, ac), sinv);
    v.x = cstd::div(FX_MUL(v.x, f1), 0x60);
    v.y = cstd::div(FX_MUL(v.y, f1), 0x60);
    InvMat4x3(cam->viewMatrix, &inv);
    MulVec3Mat4x3(&v, &inv, out);
}
}
