#include "types.h"
// @symbol func_ov002_020b781c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern short data_02082214[];

extern void func_ov002_020b6fcc(void *self);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern char *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, void *out);
extern int func_02037e58(void *p);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_ov002_020f030c(int x);
extern int Vec3_HorzLen(void *v);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern void Vec3_Add(void *out, void *a, void *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern short func_02010844(void *self, void *v, short angle);
extern void _Z11UpdateAngleRssis(void *p, short a, int step, short d);

int func_ov002_020b781c(char *c)
{
    struct Vector3 v1;
    struct Vector3 v2;
    struct Vector3 out;
    int st;
    char *fr;
    int r6;
    int r4v;
    int spd;
    short ang;
    short a1;
    short a2;
    int b;
    int j;
    int s;
    int co;

    st = *(int *)(c + 0x3f0);
    if (st == 4 || st == 0x11 || st == 6 || st == 8 || st == 0x10) {
        func_ov002_020b6fcc(c);
        return 1;
    }
    if (DecIfAbove0_Short((unsigned short *)(c + 0x100)) == 0)
        return 1;
    if (*(u16 *)(c + 0x100) == 1) {
        b = (int)((*(int *)(c + 0xb0) & 0x60000) != 0);
        if (b == 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return 1;
        }
    }
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) == 0) {
        *(int *)(c + 0x408) = *(int *)(c + 0x98);
        return 1;
    }
    func_ov002_020b6fcc(c);
    if (*(u16 *)(c + 0x100) < *(u16 *)(c + 0x404) >> 1) {
        *(int *)(c + 0x3ec) = (*(u16 *)(c + 0x100) & 4) >> 2;
        if (*(u16 *)(c + 0x100) < *(u16 *)(c + 0x404) >> 2)
            *(int *)(c + 0x3ec) = (*(u16 *)(c + 0x100) & 2) >> 1;
    }
    if (*(int *)(c + 0x3f0) == 0x12) {
        *(int *)(c + 0x98) = 0;
        return 1;
    }
    fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x144);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, c + 0xd4);
    r6 = func_02037e58(fr + 4);
    *(short *)(c + 0x3fc) = _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(c + 0xd4), *(int *)(c + 0xdc));
    r4v = func_ov002_020f02c8(r6);
    func_ov002_020f030c(r6);
    spd = *(int *)(c + 0x98);
    j = (*(u16 *)(c + 0x94) >> 4) * 2;
    s = data_02082214[j];
    co = data_02082214[j + 1];
    v1.x = (int)(((long long)spd * s + 0x800) >> 12);
    v1.y = 0;
    v1.z = (int)(((long long)spd * co + 0x800) >> 12);
    j = (*(u16 *)(c + 0x3fc) >> 4) * 2;
    s = data_02082214[j];
    co = data_02082214[j + 1];
    v2.x = (int)(((long long)r4v * s + 0x800) >> 12);
    v2.y = 0;
    v2.z = (int)(((long long)r4v * co + 0x800) >> 12);
    Vec3_MulScalarInPlace(&v2, Vec3_HorzLen(c + 0xd4));
    Vec3_Add(&out, &v1, &v2);
    ang = _ZN4cstd5atan2E5Fix12IiES1_(out.x, out.z);
    *(int *)(c + 0x98) = Vec3_HorzLen(&out);
    if (*(int *)(c + 0x98) > 0xf000)
        *(int *)(c + 0x98) = 0xf000;
    *(short *)(c + 0x94) = ang;
    *(int *)(c + 0xa8) = -(_ZN4cstd4fdivEii(
        (int)(((long long)*(int *)(c + 0xd4) * *(int *)(c + 0xa4) + 0x800) >> 12)
      + (int)(((long long)*(int *)(c + 0xdc) * *(int *)(c + 0xac) + 0x800) >> 12),
        *(int *)(c + 0xd8)) + 0x8000);
    a1 = func_02010844(c, c + 0xd4, *(short *)(c + 0x8e));
    a2 = func_02010844(c, c + 0xd4, *(short *)(c + 0x8e) - 0x4000);
    _Z11UpdateAngleRssis(c + 0x8c, a1, 4, 0x1000);
    _Z11UpdateAngleRssis(c + 0x90, a2, 4, 0x1000);
    return 1;
}
