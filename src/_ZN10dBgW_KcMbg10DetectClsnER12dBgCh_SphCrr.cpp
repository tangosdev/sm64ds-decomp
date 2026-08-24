//cpp
// @symbol _ZN10dBgW_KcMbg10DetectClsnER12dBgCh_SphCrr
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 8, and the same trick as the other two overloads: move the query
 * into the collider's local frame rather than moving the mesh. The caller's
 * sphere centre is transformed by func_02039e48 and its radius scaled by
 * unk_164, a local dBgCh_SphCrr is aimed at the result, and the base
 * dBgW_Kc::DetectClsn does the work against the static mesh.
 *
 * Coming back out is the part that is not symmetric. The push-out vector pair
 * is scaled by `scale` -- the collider's own uniform scale, NOT unk_164 which
 * scaled the radius on the way in -- and the three result slots are merged
 * INDEPENDENTLY, each behind its own bit of dBgCh_SphCrr::flags:
 *
 *   floor (4)  first writer wins. If the caller already has a floor, this one
 *              is dropped AND the return value's bit 0 is cleared, so the hit
 *              is reported as "not new". Otherwise SetFloorResult takes it.
 *   wall (8)   merged unconditionally by func_02037888.
 *   under(0x10) merged unconditionally by func_0203782c.
 *
 * The 0x100 score is compared separately from all three and can win even when
 * the floor result above was dropped.
 *
 * `#pragma opt_common_subs off` is original and load-bearing: the six FMULs
 * against `scale` are deliberately not CSE'd.
 *
 * STILL A SHADOW: the local query object. It needs an exact 0x110 footprint
 * with explicit C1/D1 calls, while dBgCh_SphCrr as declared spans 0x10c and
 * declares no structors -- giving it real ones is its own slice, since it
 * changes every by-value use of the type. Kept as a byte-exact stand-in and
 * flagged rather than half-converted.
 */
#include "types.h"
#include "dBgW_KcMbg.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"

typedef struct {
    int head[4];        /* 0x00 */
    char result[0x60];  /* 0x10 */
    u8 flags;           /* 0x70 */
    char pad71[3];
    char floorRes[0x28];/* 0x74 */
    char wallRes[0x28]; /* 0x9c */
    char undRes[0x28];  /* 0xc4 */
    int f_ec;           /* 0xec */
    int f_f0[3];        /* 0xf0 */
    int f_fc;           /* 0xfc */
    int f_100;          /* 0x100 */
    int tail[3];        /* 0x104..0x110 */
} LocSphere;

extern "C" {
extern void _ZN12dBgCh_SphCrrC1Ev(void* o);
extern void func_02039e48(void* m, void* v, void* c);
extern void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(void* o, void* v, int r, void* a);
extern void func_02037940(void* p, int v);
extern void func_02035394(void* o, void* r);
extern void func_02037a04(void* o, void* d1, void* d2);
extern void func_02037a6c(void* b, int x1, int y1, int z1, int x2, int y2, int z2);
extern void _ZN5dBgPiaSERKS_(void* d, void* s);
extern void func_0203794c(void* d, void* s);
extern void func_02037888(void* d, void* s);
extern void func_0203782c(void* d, void* s);
extern void _ZN12dBgCh_SphCrrD1Ev(void* o);
}

#pragma opt_common_subs off

#define FMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

int dBgW_KcMbg::DetectClsn(dBgCh_SphCrr & sphere_)
{
    dBgCh_SphCrr* sphere = &sphere_;
    int pos[3];
    int d[12];
    LocSphere loc;
    int r;

    _ZN12dBgCh_SphCrrC1Ev(&loc);
    func_02039e48(this, &sphere->centre, pos);
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&loc, pos,
        FMUL(*(int*)&sphere->radius, unk_164), 0);
    loc.f_ec = FMUL(sphere->unk_0ec, unk_164);
    func_02037940(&loc, sphere->flags);
    func_02035394(&loc, sphere);
    r = dBgW_Kc::DetectClsn(*(dBgCh_SphCrr*)&loc);
    if (r) {
        func_02037a04(&loc, d, d + 3);
        d[6] = FMUL(d[0], *(int*)&scale);
        d[7] = FMUL(d[1], *(int*)&scale);
        d[8] = FMUL(d[2], *(int*)&scale);
        d[9] = FMUL(d[3], *(int*)&scale);
        d[10] = FMUL(d[4], *(int*)&scale);
        d[11] = FMUL(d[5], *(int*)&scale);
        func_02037a6c(sphere, d[6], d[7], d[8], d[9], d[10], d[11]);
        /* through the REFERENCE: a pointer-level upcast makes mwcc emit the
           null-checked MI adjustment (cmp/addne), the ROM's is unconditional */
        _ZN5dBgPiaSERKS_(&(dBgPi &)sphere_, loc.result);
        *(u8*)(&sphere->flags) |= 1;
        if (loc.flags & 4) {
            if (sphere->flags & 4) {
                r &= ~1;
            } else {
                sphere->SetFloorResult(*(dBgPi*)loc.floorRes);
            }
            *(u8*)(&sphere->flags) |= 4;
            if (sphere->unk_100 < loc.f_100) {
                func_0203794c(sphere, &loc.f_fc);
            }
        }
        if (loc.flags & 8) {
            func_02037888(sphere, loc.wallRes);
            *(u8*)(&sphere->flags) |= 8;
        }
        if (loc.flags & 0x10) {
            func_0203782c(sphere, loc.undRes);
            *(u8*)(&sphere->flags) |= 0x10;
        }
    }
    _ZN12dBgCh_SphCrrD1Ev(&loc);
    return r;
}
