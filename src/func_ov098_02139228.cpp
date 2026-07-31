//cpp
// @symbol func_ov098_02139228
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;



struct C {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
};

extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
extern int func_02037e38(u32 *p);
extern void func_ov098_02138b28(void *c, int i);
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, Vector3 *out);
extern int func_02037e58(u32 *p);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern int func_ov002_020f030c(int x);
extern s32 Vec3_HorzLen(const Vector3 *v0);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, const Vector3 *pos);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, const Vector3 *pos, u32 e);
extern s16 func_02010844(void *unused, Vector3 *v, s16 angle);
extern void _Z11UpdateAngleRssis(s16 *a, s16 b, int c, s16 d);
extern s16 data_02082214[];
}

extern "C" int func_ov098_02139228(char *c)
{

#pragma opt_propagation off
    int base = 2;
    void *fr;
    int n;
    int ang;
    int newAng;
    int hl;
    int i94, iang;
    Vector3 a1;
    Vector3 a2;
    Vector3 sum;

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320) == 0)
        return 0;

    fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x320);
    switch (func_02037e38((u32 *)((char *)fr + 4))) {
    case 6:
        *(s32 *)(c + 0x5f0) = 0x1e000;
        break;
    case 7:
        *(s32 *)(c + 0x5f0) = 0x2d000;
        break;
    case 8:
        *(s32 *)(c + 0x5f0) = 0x3c000;
        break;
    case 9:
        *(s32 *)(c + 0x5f0) = 0x64000;
        if (*(s32 *)(c + 0x5f0) == *(s32 *)(c + 0x5f4)) {
            func_ov098_02138b28(c, 6);
            return 0;
        }
        break;
    }

    _Z14ApproachLinearRiii((int *)(c + 0x5f4), *(s32 *)(c + 0x5f0), 0x800);

    if ((*(s32 *)(c + 0x4e0) | *(s32 *)(c + 0x98)) == 0) {
        int *p = (int *)(((int)c + 0x57c) & 0xFFFFFFFFFFFFFFFF);
        *p &= ~0x2000;
        return 0;
    }

    if (*(s32 *)(c + 0xa8) < -0xb000) {
        ((C *)c)->v31();
    }

    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)fr + 4, (Vector3 *)(c + 0x4dc));
    n = func_02037e58((u32 *)((char *)fr + 4));
    ang = _ZN4cstd5atan2E5Fix12IiES1_(*(s32 *)(c + 0x4dc), *(s32 *)(c + 0x4e4));
    hl = func_ov002_020f02c8(n);
    func_ov002_020f030c(n);


    {
        u16 r_94 = *(u16 *)(c + 0x94);
        int sa = (u16)ang;
        sa >>= 4;
        int s94 = r_94;
        s94 >>= 4;
        int li94 = s94 * 2;
        s32 v98 = *(s32 *)(c + 0x98);
        s16 cos94 = data_02082214[li94];
        s16 sin94 = data_02082214[li94 + 1];
        a1.y = 0;
        a2.y = 0;
        s32 a1x = (s32)(((s64)v98 * cos94 + 0x800) >> 12);
        a1.z = (s32)(((s64)v98 * sin94 + 0x800) >> 12);
        s16 cosa = data_02082214[sa * 2];
        s16 sina = data_02082214[sa * 2 + 1];
        a2.x = (s32)(((s64)hl * cosa + 0x800) >> 12);
        a2.z = (s32)(((s64)hl * sina + 0x800) >> 12);
        a1.x = a1x;
    }
    Vec3_MulScalarInPlace(&a2.x, Vec3_HorzLen((Vector3 *)(c + 0x4dc)));
    Vec3_Add(&sum, &a1, &a2);
    newAng = _ZN4cstd5atan2E5Fix12IiES1_(sum.x, sum.z);
    *(s32 *)(c + 0x98) = Vec3_HorzLen(&sum);
    if (*(s32 *)(c + 0x98) > 0x64000)
        *(s32 *)(c + 0x98) = 0x64000;
    *(s16 *)(c + 0x94) = newAng;
    AngleDiff(*(s16 *)(c + 0x94), *(s16 *)(c + 0x8e));

    {
        s32 m0 = (s32)(((s64)*(s32 *)(c + 0x4dc) * *(s32 *)(c + 0xa4) + 0x800) >> 12);
        s32 m1 = (s32)(((s64)*(s32 *)(c + 0x4e4) * *(s32 *)(c + 0xac) + 0x800) >> 12);
        *(s32 *)(c + 0xa8) = -(_ZN4cstd4fdivEii(m0 + m1, *(s32 *)(c + 0x4e0)) + 0x8000);
    }

    if (func_ov002_020f035c(n, *(s32 *)(c + 0x4e0)) != 0 && *(s32 *)(c + 0x98) > 0x5000) {
        int q0 = *(s16 *)(c + 0x8c);
        int q1;
        int sd;
        if (q0 < 0) q0 = ((-q0) << 16) >> 16;
        if (q0 < 0x10) {
            q1 = *(s16 *)(c + 0x90);
            if (q1 < 0) q1 = ((-q1) << 16) >> 16;
            if (q1 < 0x10) {
                s32 v = *(s32 *)(c + 0x98);
                sd = _ZN4cstd4fdivEii((s32)(((s64)v * 8 + 0x800) >> 12), 0xa);
                if (sd < 0) sd = -sd;
                if (*(s32 *)(c + 0xa8) > 0xa000)
                    _ZN5Sound9PlayBank3EjRK7Vector3(0x51, (Vector3 *)(c + 0x74));
                if (*(s32 *)(c + 0xa8) > sd)
                    *(s32 *)(c + 0xa8) = sd;
            }
        }
    }

    if (_Z14ApproachLinearRiii((int *)(c + 0x98), 0, 0x800) == 0
        && *(s32 *)(c + 0x5f0) != 0xa0
        && *(s32 *)(c + 0x98) > 0xa000) {
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(s32 *)(c + 0x5c), *(s32 *)(c + 0x60), *(s32 *)(c + 0x64));
        *(u32 *)(c + 0x5f8) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(u32 *)(c + 0x5f8), 3, 0x93, (Vector3 *)(c + 0x74), 0);
    }

    {
        s16 r5 = func_02010844(c, (Vector3 *)(c + 0x4dc), *(s16 *)(c + 0x8e));
        s16 r4 = func_02010844(c, (Vector3 *)(c + 0x4dc), (s16)(*(s16 *)(c + 0x8e) - 0x4000));
        _Z11UpdateAngleRssis((s16 *)(c + 0x8c), r5, 4, 0x1000);
        _Z11UpdateAngleRssis((s16 *)(c + 0x90), r4, 4, 0x1000);
    }
    return 1 | (base & 0);
}
