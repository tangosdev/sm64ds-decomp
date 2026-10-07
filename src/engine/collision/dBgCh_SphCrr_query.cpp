//cpp
/* dBgCh_SphCrr -- the query half: DetectClsn and its two registry-scan
 * variants. The cartridge emits these three at 0x02038824..0x02038ea4,
 * apart from the lifecycle/accessor run (dBgCh_SphCrr.cpp) which ends at
 * 0x02037dc4 -- the SurfaceInfo/dBgPi/dBgPc block is linked between them.
 *
 * data_020a0c80 is dBgW's enabled-collider table (0x18 entries). A scan
 * skips the collider the query is already bound to (func_02035354), checks
 * the sphere against the collider OWNER's position and clip radius while
 * the owner is active, then dispatches the collider's own
 * dBgW::DetectClsn(dBgCh_SphCrr&) -- vtable slot 8 -- and records the
 * floor / wall / underneath hits it reports.
 */
#include "dBgCh_SphCrr.h"
#include "dBgW.h"
#include "dActor_c.h"

extern "C" {
int  func_02035354(void *a, void *b);       /* query already bound to this owner? */
int  func_020393ac(void *o);                /* collider -> ownerUniqueID */
int  func_020393b4(void *o);                /* collider -> owner (dActor_c*) */
int  func_0203938c(void *o);                /* collider -> +0x10 */
int  func_0203939c(void *o);                /* collider -> +0x0c */
int  Vec3_Dist(const Vector3 *a, const Vector3 *b);
dBgW *data_020a0c80[];                      /* enabled-collider table, 0x18 slots */
}

// @symbol _ZN12dBgCh_SphCrr10DetectClsnEv
int dBgCh_SphCrr::DetectClsn()
{
    int result = 0;
    int mask;
    char *e = (char *)data_020a0c80[0];
    if (e != 0) {
        if (func_02035354(this, (void *)func_020393b4(e)) == 0) {
            mask = ((dBgW *)e)->DetectClsn(*this);
            if (mask != 0) {
                ((dBgPi *)((char *)this + 0x10))->SetCollider(0, func_020393ac(e),
                    (dActor_c *)func_020393b4(e), (dBgW *)e);
                if (mask & 1)
                    func_020379d0(0, func_020393ac(e), func_020393b4(e), (int)e);
                if (mask & 2)
                    func_0203799c(0, func_020393ac(e), func_020393b4(e), (int)e);
                if (mask & 4)
                    func_02037968(0, func_020393ac(e), func_020393b4(e), (int)e);
                result = 1;
            }
        }
    }

    e = (char *)this + 0x3c;
    int threshold = *(int *)((char *)this + 0x48) + 0x1000;
    for (int i = 1; i < 0x18; i++) {
        dBgW *o = data_020a0c80[i];
        if (o == 0) continue;
        dActor_c *owner = (dActor_c *)func_020393b4(o);
        if (func_02035354(this, owner) != 0) continue;
        if (owner != 0) {
            int active = (owner->mFlags & 2) ? 1 : 0;
            if (active != 0) {
                Vector3 v;
                Vector3 *src = (Vector3 *)&owner->mPosX;
                v.x = src->x;
                v.y = src->y;
                v.z = src->z;
                mask = func_0203939c(o);
                if (mask == -0x1000) {
                    int b4 = owner->mClipOffsetY;
                    int b8 = owner->mClipRadius;
                    v.y += b4;
                    mask = b8 << 3;
                } else {
                    v.y += func_0203938c(o);
                }
                if (Vec3_Dist((const Vector3 *)e, &v) > mask + threshold)
                    continue;
            }
        }
        {
            { int t = ((dBgW *)o)->DetectClsn(*this); mask = t; }
            if (mask != 0) {
                ((dBgPi *)((char *)this + 0x10))->SetCollider(i, func_020393ac(o),
                    (dActor_c *)func_020393b4(o), (dBgW *)o);
                if (mask & 1)
                    func_020379d0(i, func_020393ac(o), func_020393b4(o), (int)o);
                if (mask & 2)
                    func_0203799c(i, func_020393ac(o), func_020393b4(o), (int)o);
                if (mask & 4)
                    func_02037968(i, func_020393ac(o), func_020393b4(o), (int)o);
                result = 1;
            }
        }
    }
    this->func_02037a38();
    return result;
}

// @symbol _ZN12dBgCh_SphCrr13func_02038a38Ev
int dBgCh_SphCrr::func_02038a38()
{
    dBgW *o = data_020a0c80[0];
    int ret = 0;
    int flags;
    if (o != 0 && func_02035354(this, (void *)func_020393b4(o)) == 0
        && (flags = o->DetectClsn(*this)) != 0) {
        ((dBgPi *)((char *)this + 0x10))->SetCollider(0, func_020393ac(o),
            (dActor_c *)func_020393b4(o), (dBgW *)o);
        if (flags & 1)
            func_020379d0(0, func_020393ac(o), func_020393b4(o), (int)o);
        if (flags & 2)
            func_0203799c(0, func_020393ac(o), func_020393b4(o), (int)o);
        if (flags & 4)
            func_02037968(0, func_020393ac(o), func_020393b4(o), (int)o);
        ret = 1;
    }
    this->func_02037a38();
    return ret;
}

// @symbol _ZN12dBgCh_SphCrr13func_02038824Ev
int dBgCh_SphCrr::func_02038824()
{
    int limit = *(int *)((char *)this + 0x48) + 0x1000;
    int ret = 0;
    int zero = 0;
    int one = 1;
    int i;
    for (i = 1; i < 0x18; i++) {
        dBgW *o = data_020a0c80[i];
        if (o == 0) continue;
        dActor_c *sl = (dActor_c *)func_020393b4(o);
        if (func_02035354(this, sl) != 0) continue;
        if (sl != 0 && ((sl->mFlags & 2) ? one : zero)) {
            /* Lever (notes 6ar): the hoisted address local is declared
               `int pos = (int)this + 0x3c`, NOT `char *pos = ...`. The local's
               DECLARED TYPE is the lever, not a cast on the expression; a
               pointer local only reaches 12 diffs. */
            int pos = (int)this + 0x3c;
            Vector3 v;
            Vector3 *src = (Vector3 *)&sl->mPosX;
            int r5;
            v.x = src->x;
            v.y = src->y;
            v.z = src->z;
            r5 = func_0203939c(o);
            if (r5 == -0x1000) {
                int hi = sl->mClipRadius;
                v.y = v.y + sl->mClipOffsetY;
                r5 = hi << 3;
            } else {
                v.y = v.y + func_0203938c(o);
            }
            if (Vec3_Dist((const Vector3 *)pos, &v) > r5 + limit) continue;
        }
        int flags = o->DetectClsn(*this);
        if (flags == 0) continue;
        ((dBgPi *)((char *)this + 0x10))->SetCollider(i, func_020393ac(o),
            (dActor_c *)func_020393b4(o), (dBgW *)o);
        if (flags & 1) func_020379d0(i, func_020393ac(o), func_020393b4(o), (int)o);
        if (flags & 2) func_0203799c(i, func_020393ac(o), func_020393b4(o), (int)o);
        if (flags & 4) func_02037968(i, func_020393ac(o), func_020393b4(o), (int)o);
        ret = one;
    }
    this->func_02037a38();
    return ret;
}
