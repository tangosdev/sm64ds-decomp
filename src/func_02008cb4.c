#include "types.h"
enum { false, true };

typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct RaycastLine { char pad[0x14]; char surf[0x64]; } RaycastLine;

extern int func_ov002_020ca270(char* p);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalarInPlace(Vec3* v, int s);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern void _ZN11RaycastLineC1Ev(RaycastLine* rc);
extern void func_0200897c(char* self, RaycastLine* rc);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(RaycastLine* rc, Vec3* a, Vec3* b, void* actor);
extern int _ZN11RaycastLine10DetectClsnEv(RaycastLine* rc);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* surf, Vec3* out);
extern int DotVec3(Vec3* a, Vec3* b);
extern void _ZN11RaycastLineD1Ev(RaycastLine* rc);
extern void _ZN11RaycastLine10GetClsnPosEv(Vec3* out, RaycastLine* rc);
extern int Vec3_Dist(Vec3* a, Vec3* b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int AngleDiff(int a, int b);

int func_02008cb4(char* c)
{
    Vec3 v0;
    Vec3 dir1;
    Vec3 end1;
    Vec3 n1;
    Vec3 dir2;
    Vec3 end2;
    Vec3 n2;
    Vec3 diff;
    Vec3 cp1;
    Vec3 cp2;
    Vec3 np;
    Vec3 cl;
    RaycastLine rc1;
    RaycastLine rc2;
    int vy;
    int vz;
    int vx;
    int scale;
    int len;
    int d;

    {
        char* p = *(char**)(c + 0x110);
        int b = *(u16*)(p + 0xc);
        b = b == 0xbf;
        if (b != false) {
            if (func_ov002_020ca270(p) != 0)
                return 0;
        }
    }

    scale = 0x200;
    if (*(u32*)(c + 0x154) & 0x40200) {
        vz = *(int*)(c + 0xa0);
        vy = *(int*)(c + 0x9c) + 0x82500;
        vx = *(int*)(c + 0x98);
        v0.x = vx;
        v0.y = vy;
        v0.z = vz;
        Vec3_Sub(&dir1, (Vec3*)(c + 0x8c), &v0);
        len = LenVec3(&dir1);
        if (len != 0) {
            Vec3_MulScalarInPlace(&dir1, _ZN4cstd4fdivEii(0x100000, len));
            Vec3_Add(&end1, (Vec3*)(c + 0x8c), &dir1);
            _ZN11RaycastLineC1Ev(&rc1);
            func_0200897c(c, &rc1);
            _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&rc1, &end1, &v0, 0);
            if (_ZN11RaycastLine10DetectClsnEv(&rc1) != 0) {
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rc1.surf, &n1);
                if (DotVec3((Vec3*)(c + 0xec), &n1) > 0) {
                    *(u32*)(((int)c + 0x154)) &= ~0x80000;
                    _ZN11RaycastLineD1Ev(&rc1);
                    return 0;
                }
                _ZN11RaycastLine10GetClsnPosEv(&cp1, &rc1);
                if (Vec3_Dist((Vec3*)(c + 0xe0), &cp1) < 0x80000) {
                    *(u32*)(((int)c + 0x154)) &= ~0x80000;
                    _ZN11RaycastLineD1Ev(&rc1);
                    return 0;
                }
                {
                    u32 f = *(u32*)(c + 0x154);
                    if ((f & 0x40000) && *(u8*)(c + 0x1a6) != 0 && !(f & 0x80100)) {
                        int a = _ZN4cstd5atan2E5Fix12IiES1_(n1.x, n1.z);
                        if (AngleDiff(a, _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(c + 0xec), *(int*)(c + 0xf4))) > 0x6000) {
                            *(s16*)(((int)c + 0x186)) += 0x8000;
                            *(u32*)(((int)c + 0x154)) |= 0x80000;
                        }
                    }
                }
            }
            _ZN11RaycastLineD1Ev(&rc1);
        }
        scale = 0x1000;
    }

    {
        u32 f = *(u32*)(c + 0x154);
        if (f & 0x80100) {
            vz = *(int*)(c + 0xa0);
            vy = (f & 0x100) ? *(int*)(c + 0x90) : 0x82500 + *(int*)(c + 0x9c);
            vx = *(int*)(c + 0x98);
            v0.x = vx;
            v0.y = vy;
            v0.z = vz;
        } else {
            scale = 0x1000;
            v0.x = *(int*)(c + 0xd4);
            v0.y = *(int*)(c + 0xd8);
            v0.z = *(int*)(c + 0xdc);
        }
    }

    Vec3_Sub(&dir2, (Vec3*)(c + 0x8c), &v0);
    len = LenVec3(&dir2);
    if (len == 0) {
        *(u32*)(((int)c + 0x154)) &= ~0x80000;
        return 0;
    }
    Vec3_MulScalarInPlace(&dir2, _ZN4cstd4fdivEii(0x60000, len));
    Vec3_Add(&end2, (Vec3*)(c + 0x8c), &dir2);
    _ZN11RaycastLineC1Ev(&rc2);
    func_0200897c(c, &rc2);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&rc2, &v0, &end2, 0);
    if (_ZN11RaycastLine10DetectClsnEv(&rc2) == 0) {
        *(u32*)(((int)c + 0x154)) &= ~0x80000;
        _ZN11RaycastLineD1Ev(&rc2);
        return 0;
    }
    if (!(*(u32*)(c + 0x154) & 0x80100)) {
        int t;
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rc2.surf, &n2);
        t = n2.y;
        if (t < 0) t = -t;
        if (t > 0xb50) {
            _ZN11RaycastLineD1Ev(&rc2);
            return 0;
        }
    }
    d = Vec3_Dist((Vec3*)(c + 0x8c), (Vec3*)(c + 0x80));
    Vec3_MulScalarInPlace(&dir2, scale);
    _ZN11RaycastLine10GetClsnPosEv(&cp2, &rc2);
    Vec3_Sub(&np, &cp2, &dir2);
    *(int*)(c + 0x8c) = np.x;
    *(int*)(c + 0x90) = np.y;
    *(int*)(c + 0x94) = np.z;
    Vec3_Sub(&diff, (Vec3*)(c + 0x8c), (Vec3*)(c + 0x80));
    if (LenVec3(&diff) >= d) {
        Vec3_MulScalarInPlace(&diff, _ZN4cstd4fdivEii(d, LenVec3(&diff)));
        Vec3_Add(&cl, (Vec3*)(c + 0x80), &diff);
        *(int*)(c + 0x8c) = cl.x;
        *(int*)(c + 0x90) = cl.y;
        *(int*)(c + 0x94) = cl.z;
    }
    if (*(u8*)(c + 0x1a6) != 0 || *(u16*)(c + 0x1a0) != 0) {
        *(u8*)(c + 0x1a6) = 0;
        *(u16*)(c + 0x1a0) = 0;
    }
    *(u32*)(((int)c + 0x154)) &= ~0x80000;
    _ZN11RaycastLineD1Ev(&rc2);
    return 1;
}
