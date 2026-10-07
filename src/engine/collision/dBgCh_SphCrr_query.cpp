//cpp
// The sphere query's registry scans: walk dBgW's enabled-collider table
// (data_020a0c80, 0x18 entries), skip the collider this query is already
// bound to, cull against the collider owner's clip volume, then dispatch the
// collider's own DetectClsn(dBgCh_SphCrr&) -- vtable slot 8 -- and record
// the floor / wall / underneath hits it reports.
//
// comment leftovers:
//   - func_02035354 stays a free extern: the shared dBgCh-side bound-to
//     check is owned by its shard and called by the Gnd/Lin scans as well.
//   - func_0203938c/9c/ac/b4 stay free externs: weak dBgW field accessors
//     owned by dBgW.cpp (+0x10/+0xc clip overrides, +8 ownerUniqueID,
//     +4 owner); the bl is the code.
//   - DetectClsn's `char *e` doubles as the slot-0 collider and the centre
//     pointer, and `hits` doubles as the clip-distance temp; splitting
//     either changes the register shape, as does dropping the `{ int t =
//     ...; hits = t; }` block at the dispatch.
//   - `int pos` in func_02038824 keeps its declared type (the lever is int,
//     not a pointer local); its value is the dM3dGSph base's centre. The
//     `zero`/`one` constant locals in the same scan are the register
//     materialization -- `? 1 : 0` restructures the whole function.
//   - data_020a0c80 keeps its address name: the enabled-collider table is
//     tree-wide data_ (dBgW.cpp spells it void*[], a banked disagreement).
//   - Vec3_Dist stays a local extern: no shared header declares it.
#include "dBgCh_SphCrr.h"
#include "dBgW.h"
#include "dActor_c.h"

extern "C" {
int  func_02035354(void *a, void *b);       /* local extern: shared helper --
                                               query already bound to actor? */
int  func_020393ac(void *o);                /* local extern: dBgW +8 ownerUniqueID */
int  func_020393b4(void *o);                /* local extern: dBgW +4 owner */
int  func_0203938c(void *o);                /* local extern: dBgW +0x10 clip y */
int  func_0203939c(void *o);                /* local extern: dBgW +0xc clip dist */
int  Vec3_Dist(const Vector3 *a, const Vector3 *b); /* local extern: undeclared */
dBgW *data_020a0c80[];                      /* enabled-collider table, 0x18 slots */
}

// @symbol _ZN12dBgCh_SphCrr10DetectClsnEv
int dBgCh_SphCrr::DetectClsn()
{
    int result = 0;
    int hits;

    /* slot 0 first, then the rest of the table; e is reused for the sphere
       centre below -- the one local keeps the compiler's register shape */
    char *e = (char *)data_020a0c80[0];
    if (e != 0) {
        if (func_02035354(this, (void *)func_020393b4(e)) == 0) {
            hits = ((dBgW *)e)->DetectClsn(*this);
            if (hits != 0) {
                ((dBgPi &)*this).SetCollider(0, func_020393ac(e),
                    (dActor_c *)func_020393b4(e), (dBgW *)e);
                if (hits & 1)
                    func_020379d0(0, func_020393ac(e), (dActor_c *)func_020393b4(e), (dBgW *)e);
                if (hits & 2)
                    func_0203799c(0, func_020393ac(e), (dActor_c *)func_020393b4(e), (dBgW *)e);
                if (hits & 4)
                    func_02037968(0, func_020393ac(e), (dActor_c *)func_020393b4(e), (dBgW *)e);
                result = 1;
            }
        }
    }

    e = (char *)&((dM3dGSph &)*this).centre;
    int reach = ((dM3dGSph &)*this).radius + 0x1000;
    for (int i = 1; i < 0x18; i++) {
        dBgW *o = data_020a0c80[i];
        if (o == 0) continue;
        dActor_c *owner = (dActor_c *)func_020393b4(o);
        if (func_02035354(this, owner) != 0) continue;
        if (owner != 0) {
            int active = (owner->mFlags & 2) ? 1 : 0;
            if (active != 0) {
                Vector3 ownerPos;
                Vector3 *src = (Vector3 *)&owner->mPosX;
                ownerPos.x = src->x;
                ownerPos.y = src->y;
                ownerPos.z = src->z;
                hits = func_0203939c(o);
                if (hits == -0x1000) {
                    int clipOffsetY = owner->mClipOffsetY;
                    int clipRadius = owner->mClipRadius;
                    ownerPos.y += clipOffsetY;
                    hits = clipRadius << 3;
                } else {
                    ownerPos.y += func_0203938c(o);
                }
                if (Vec3_Dist((const Vector3 *)e, &ownerPos) > hits + reach)
                    continue;
            }
        }
        { int t = ((dBgW *)o)->DetectClsn(*this); hits = t; }
        if (hits != 0) {
            ((dBgPi &)*this).SetCollider(i, func_020393ac(o),
                (dActor_c *)func_020393b4(o), o);
            if (hits & 1)
                func_020379d0(i, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
            if (hits & 2)
                func_0203799c(i, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
            if (hits & 4)
                func_02037968(i, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
            result = 1;
        }
    }
    this->func_02037a38();
    return result;
}

// @symbol _ZN12dBgCh_SphCrr13func_02038a38Ev
int dBgCh_SphCrr::func_02038a38()
{
    /* same scan against slot 0 only */
    dBgW *o = data_020a0c80[0];
    int result = 0;
    int hits;
    if (o != 0 && func_02035354(this, (void *)func_020393b4(o)) == 0
        && (hits = o->DetectClsn(*this)) != 0) {
        ((dBgPi &)*this).SetCollider(0, func_020393ac(o),
            (dActor_c *)func_020393b4(o), o);
        if (hits & 1)
            func_020379d0(0, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
        if (hits & 2)
            func_0203799c(0, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
        if (hits & 4)
            func_02037968(0, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
        result = 1;
    }
    this->func_02037a38();
    return result;
}

// @symbol _ZN12dBgCh_SphCrr13func_02038824Ev
int dBgCh_SphCrr::func_02038824()
{
    /* the full-table scan variant: colliders 1..0x17 only */
    int reach = ((dM3dGSph &)*this).radius + 0x1000;
    int result = 0;
    int zero = 0;
    int one = 1;
    int i;
    for (i = 1; i < 0x18; i++) {
        dBgW *o = data_020a0c80[i];
        if (o == 0) continue;
        dActor_c *owner = (dActor_c *)func_020393b4(o);
        if (func_02035354(this, owner) != 0) continue;
        if (owner != 0 && ((owner->mFlags & 2) ? one : zero)) {
            /* The address local must stay declared int -- a pointer local
               diffs. Its value is the dM3dGSph base's centre. */
            int pos = (int)&((dM3dGSph &)*this).centre;
            Vector3 ownerPos;
            Vector3 *src = (Vector3 *)&owner->mPosX;
            int clipDist;
            ownerPos.x = src->x;
            ownerPos.y = src->y;
            ownerPos.z = src->z;
            clipDist = func_0203939c(o);
            if (clipDist == -0x1000) {
                int clipRadius = owner->mClipRadius;
                ownerPos.y = ownerPos.y + owner->mClipOffsetY;
                clipDist = clipRadius << 3;
            } else {
                ownerPos.y = ownerPos.y + func_0203938c(o);
            }
            if (Vec3_Dist((const Vector3 *)pos, &ownerPos) > clipDist + reach) continue;
        }
        int hits = o->DetectClsn(*this);
        if (hits == 0) continue;
        ((dBgPi &)*this).SetCollider(i, func_020393ac(o),
            (dActor_c *)func_020393b4(o), o);
        if (hits & 1) func_020379d0(i, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
        if (hits & 2) func_0203799c(i, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
        if (hits & 4) func_02037968(i, func_020393ac(o), (dActor_c *)func_020393b4(o), o);
        result = one;
    }
    this->func_02037a38();
    return result;
}
