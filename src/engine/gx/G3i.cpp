//cpp
/* engine/gx/G3i.cpp — G3i TU (arm9 0x02055998..0x02055dec)
 *
 * G3i is the geometry engine's matrix-command writer set: LookAt_ builds a
 * camera basis (right/up2/forward) and streams it to the position-matrix port,
 * PerspectiveW_ drives the division unit at 0x4000290 to build a perspective
 * projection and streams it to the projection port. Both are static (no this)
 * and both optionally mirror the matrix into a caller's Matrix4x3.
 */
// leftovers: PerspectiveW_ keeps its literal-mangled extern "C" name and C
// body with an int * matrix (the six by-value Fix12<int> params are the
// include/OAM.h ABI wall; the ROM bytes are the C front end's output).
// func_02055998 keeps its address name (no recovered name exists).
#include "G3i.h"
#include "math/Matrix.h"
#include "nitro/fx/fx.h"

extern "C" {
void NormalizeVec3(Vector3 *v, Vector3 *out);
void CrossVec3(const Vector3 *a, const Vector3 *b, Vector3 *out);
Fix12i DotVec3(const Vector3 *a, const Vector3 *b);
int _ZN4cstd4fdivEii(int, int);
int _ZN4cstd11fdiv_resultEv(void);
s64 _ZN4cstd11ldiv_resultEv(void);
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    int sinF, int cosF, int aspect, int n, int f, int scaleW, int flag,
    int *mtx);
}

// @symbol _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3
#pragma cplusplus off
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    int sinF, int cosF, int aspect, int n, int f, int scaleW, int flag,
    int *mtx)
{
    int tmp;
    volatile int *port;
    int xx;
    s64 q;
    int zz;
    int v;
    int wz;

    tmp = _ZN4cstd4fdivEii(cosF, sinF);
    if (scaleW != 0x1000)
        tmp = tmp * scaleW / 0x1000;

    *(volatile u64 *)0x4000290 = (u64)(unsigned)tmp << 32;
    *(volatile u64 *)0x4000298 = (unsigned)aspect;
    if (flag) {
        *(volatile int *)0x4000440 = 0;
        port = (volatile int *)0x4000458;
    }
    if (mtx) {
        mtx[1] = 0;
        mtx[2] = 0;
        mtx[3] = 0;
        mtx[4] = 0;
        mtx[6] = 0;
        mtx[7] = 0;
        mtx[8] = 0;
        mtx[9] = 0;
        mtx[11] = -scaleW;
        mtx[12] = 0;
        mtx[13] = 0;
        mtx[15] = 0;
    }
    xx = _ZN4cstd11fdiv_resultEv();
    *(volatile u64 *)0x4000290 = (u64)0x1000 << 32;
    *(volatile u64 *)0x4000298 = (unsigned)(n - f);
    if (flag) {
        *port = xx;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = tmp;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = 0;
    }
    if (mtx) {
        mtx[0] = xx;
        mtx[5] = tmp;
    }
    q = _ZN4cstd11ldiv_resultEv();
    if (scaleW != 0x1000)
        q = q * scaleW / 0x1000;
    zz = (int)(((f + n) * q + 0x80000000LL) >> 32);
    v = FX_MUL((n << 1), f);
    wz = (int)((q * v + 0x80000000LL) >> 32);
    if (flag) {
        *port = zz;
        *port = -scaleW;
        *port = 0;
        *port = 0;
        *port = wz;
        *port = 0;
    }
    if (mtx) {
        mtx[10] = zz;
        mtx[14] = wz;
    }
}
#pragma cplusplus on

// @symbol _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3
void G3i::LookAt_(const Vector3 *at, const Vector3 *up, const Vector3 *eye,
                  bool draw, Matrix4x3 *mat)
{
    Vector3 forward;
    Vector3 right;
    Vector3 up2;
    Fix12i tx;
    Fix12i ty;
    Fix12i tz;
    volatile int *mtx;

    forward.x = at->x - eye->x;
    forward.y = at->y - eye->y;
    forward.z = at->z - eye->z;
    NormalizeVec3(&forward, &forward);

    CrossVec3(up, &forward, &right);
    NormalizeVec3(&right, &right);

    CrossVec3(&forward, &right, &up2);

    if (draw) {
        *(volatile int *)0x4000440 = 2;
        mtx = (volatile int *)0x400045c;
        *mtx = right.x;
        *mtx = up2.x;
        *mtx = forward.x;
        *mtx = right.y;
        *mtx = up2.y;
        *mtx = forward.y;
        *mtx = right.z;
        *mtx = up2.z;
        *mtx = forward.z;
    }

    tx = -DotVec3(at, &right);
    ty = -DotVec3(at, &up2);
    tz = -DotVec3(at, &forward);

    if (draw) {
        *mtx = tx;
        *mtx = ty;
        *mtx = tz;
    }

    if (mat == 0) {
        return;
    }

    mat->r.m[0] = right.x;
    mat->r.m[1] = up2.x;
    mat->r.m[2] = forward.x;
    mat->r.m[3] = right.y;
    mat->r.m[4] = up2.y;
    mat->r.m[5] = forward.y;
    mat->r.m[6] = right.z;
    mat->r.m[7] = up2.z;
    mat->r.m[8] = forward.z;
    mat->t.x = tx;
    mat->t.y = ty;
    mat->t.z = tz;
}

// @symbol func_02055998
extern "C" void func_02055998(int *m)
{
    *(volatile int *)0x4000440 = 3;
    *(volatile int *)0x4000458 = m[0] << 4;
    *(volatile int *)0x4000458 = m[1] << 4;
    *(volatile int *)0x4000458 = m[2] << 4;
    *(volatile int *)0x4000458 = m[3] << 4;
    *(volatile int *)0x4000458 = m[4] << 4;
    *(volatile int *)0x4000458 = m[5] << 4;
    *(volatile int *)0x4000458 = m[6] << 4;
    *(volatile int *)0x4000458 = m[7] << 4;
    *(volatile int *)0x4000458 = m[8] << 4;
    *(volatile int *)0x4000458 = m[9] << 4;
    *(volatile int *)0x4000458 = m[10] << 4;
    *(volatile int *)0x4000458 = m[11] << 4;
    *(volatile int *)0x4000458 = m[12];
    *(volatile int *)0x4000458 = m[13];
    *(volatile int *)0x4000458 = m[14];
    *(volatile int *)0x4000458 = m[15];
}
