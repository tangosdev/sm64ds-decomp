//cpp
// @symbol _ZN10dBgW_KcMbg10DetectClsnER12dBgCh_SphCrr
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 8, and the same trick as the other two overloads: move the query
 * into the collider's local frame rather than moving the mesh. The caller's
 * sphere centre is transformed by func_02039e48 and its radius scaled by
 * invScale, a local dBgCh_SphCrr is aimed at the result, and the base
 * dBgW_Kc::DetectClsn does the work against the static mesh.
 *
 * Coming back out is the part that is not symmetric. The push-out vector pair
 * is scaled by `scale` -- the collider's own uniform scale, NOT invScale which
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
 * The scratch query is now the real dBgCh_SphCrr. Its 0x110-byte layout and
 * C1/D1 lifecycle are both reconstructed, so ordinary automatic storage emits
 * the same constructor/destructor calls without a local shadow type.
 */
#include "types.h"
#include "dBgW_KcMbg.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"

extern "C" {
extern void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
    dBgCh_SphCrr *sphere, const Vector3 *pos, Fix12i radius, dActor_c *actor);
extern void func_02039e48(dBgW_KcMbg *self, const Vector3 *v, Vector3 *res);
extern void func_02037940(dBgCh_SphCrr *sphere, u8 flags);
extern void func_02035394(dBgCh_SphCrr *dst, dBgCh_SphCrr *src);
extern void func_02037a04(dBgCh_SphCrr *sphere, int *first, int *second);
extern void func_02037a6c(dBgCh_SphCrr *sphere,
    int x1, int y1, int z1, int x2, int y2, int z2);
extern void func_0203794c(dBgCh_SphCrr *sphere, s32 *result);
extern void func_02037888(dBgCh_SphCrr *sphere, dBgPi *result);
extern void func_0203782c(dBgCh_SphCrr *sphere, dBgPi *result);
}

#pragma opt_common_subs off

#define FMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

int dBgW_KcMbg::DetectClsn(dBgCh_SphCrr &sphere)
{
    Vector3 pos;
    int d[12];
    dBgCh_SphCrr loc;
    int r;

    func_02039e48(this, &sphere.centre, &pos);
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&loc, &pos,
        FMUL(sphere.radius, invScale), 0);
    loc.unk_0ec = FMUL(sphere.unk_0ec, invScale);
    func_02037940(&loc, sphere.flags);
    func_02035394(&loc, &sphere);
    r = dBgW_Kc::DetectClsn(loc);
    if (r) {
        func_02037a04(&loc, d, d + 3);
        d[6] = FMUL(d[0], scale);
        d[7] = FMUL(d[1], scale);
        d[8] = FMUL(d[2], scale);
        d[9] = FMUL(d[3], scale);
        d[10] = FMUL(d[4], scale);
        d[11] = FMUL(d[5], scale);
        func_02037a6c(&sphere, d[6], d[7], d[8], d[9], d[10], d[11]);
        (dBgPi &)sphere = (dBgPi &)loc;
        sphere.flags |= 1;
        if (loc.flags & 4) {
            if (sphere.flags & 4) {
                r &= ~1;
            } else {
                sphere.SetFloorResult(loc.mClsnResult1);
            }
            sphere.flags |= 4;
            if (sphere.unk_100 < loc.unk_100) {
                func_0203794c(&sphere, &loc.unk_0fc);
            }
        }
        if (loc.flags & 8) {
            func_02037888(&sphere, &loc.mClsnResult2);
            sphere.flags |= 8;
        }
        if (loc.flags & 0x10) {
            func_0203782c(&sphere, &loc.mClsnResult3);
            sphere.flags |= 0x10;
        }
    }
    return r;
}
