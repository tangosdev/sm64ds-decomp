//cpp
/* Collision middle run, arm9 0x02037dc4..0x02039140.
 * func_02037dc4 opens it and dBgCh_Gnd::DetectClsn closes it.
 * dBgW::Disable at 0x02039140 starts the next file.
 * d_bg_ch_lin.cpp and d_bg_ch_sph_crr.cpp stay separate: their this-adjustment
 * thunks are anchored at the end of those files, and this stretch has none.
 *
 * Deferred codegen emits .text in reverse source order, so the definitions
 * below run ROM-descending. dBgPc, dBgPi and CLPS_BlockRef keep the
 * lifecycle order the compiler emits for each class.
 *
 * The query scans keep the spellings that match: int pos rather than a
 * pointer local, the zero/one locals, and data_020a0c80 by its address
 * name. func_02037de8 copies the five-word surface with a u64 so the
 * loads and stores interleave. RecordHit stays the mangled call because
 * the member's s16 parameter would sign-extend here.
 */

#include "dBgCh_Lin.h"
#include "dBgCh_SphCrr.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Actr.h"
#include "dBgPc.h"
#include "dBgPi.h"
#include "CLPS_BlockRef.h"
#include "dBgW.h"
#include "dActor_c.h"
#include "SurfaceInfo.h"
#include "common.h"

extern "C" {
void func_020353b0(void *ray, void *actor);
int func_02035354(void *a, void *b);
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
int func_020393b4(void *p);
int func_020393ac(void *p);
int func_0203939c(void *p);
int func_0203938c(void *p);
int func_020393cc(void *p);
int func_020393bc(void *p);
void func_02037464(void *c);
void func_02037e9c(char *p);
void func_02037de8(int *dst, int *src);
int func_02038298(void *arg);
// local extern: forwarders pass int triID; the member's short parameter would sign-extend
void _ZN5dBgPi9RecordHitEsP11SurfaceInfo(void *res, int triID, void *info);
void func_02036acc(void);
void func_ov002_020fea4c(int *a, int *b);       /* dM3dGLin::GetEnd */
void func_ov002_020fea68(int *a, int *b);       /* dM3dGLin::GetStart */
extern dBgW *data_020a0c80[];
extern char data_020a0c78;
}

/* Ground and line scans use the real dBgW virtual interface and dActor_c
   clip-volume fields, as the sphere scans do below. Vec3 remains the plain
   three-word copy view. PcAB/PcObj describe func_02037eb0's records;
   PFN retains its pending stored-callback reconstruction. Position reads
   keep the measured three-word byte view of the actor: direct mPosX/Y/Z
   accesses remove one address-setup instruction from each of three scans. */
struct Vec3 { int x, y, z; };

struct PcAB { int a; int b; };
struct PcObj { int a; int b; struct Vector3 v; };

typedef void (*PFN)(int self, int arg, int b);

extern "C" int Vec3_HorzDist(const Vector3*, const Vector3*);

// @symbol _ZN9dBgCh_Gnd10DetectClsnEv
int dBgCh_Gnd::DetectClsn()
{
    int ret = 0;
    func_02037464(this);
    dBgW* o = (dBgW *)data_020a0c80[0];
    if (o != 0 && func_02035354(this, (void *)func_020393b4(o)) == 0 && o->DetectClsn(*this) != 0) {
        dBgPi::SetCollider(0, func_020393ac(o), (dActor_c *)func_020393b4(o), (dBgW *)o);
        ret = 1;
    }
    int flag = 1;
    int one = 1;
    Vec3 pos;
    for (int i = 1; i < 24; i++) {
        dBgW* obj = (dBgW *)data_020a0c80[i];
        if (obj == 0) continue;
        dActor_c* p = (dActor_c *)func_020393b4(obj);
        if (func_02035354(this, p) != 0) continue;
        if (p != 0 && ((p->mFlags & 2) ? flag : 0) != 0) {
            const char *position = (const char *)p + 0x5c;
            pos.x = *(const s32 *)(position + 0);
            pos.y = *(const s32 *)(position + 4);
            pos.z = *(const s32 *)(position + 8);
            int thr = func_0203939c(obj);
            if (thr == -0x1000) {
                int h8 = p->mClipRadius;
                pos.y = pos.y + p->mClipOffsetY;
                thr = h8 << 3;
            } else {
                pos.y = pos.y + func_0203938c(obj);
            }
            Vector3* selfpos = (Vector3*)((long long)(int)&this->pos);
            if (selfpos->y < pos.y - thr) continue;
            if (Vec3_HorzDist(selfpos, (const Vector3 *)&pos) > thr) continue;
        }
        if (obj->DetectClsn(*this) != 0) {
            dBgPi::SetCollider(i, func_020393ac(obj), (dActor_c *)func_020393b4(obj), (dBgW *)obj);
            ret = one;
        }
    }
    return ret;
}

// @symbol _ZN9dBgCh_Gnd15DetectClsnSlot0Ev
/* Inferred name: reset the ground query and scan only collider slot zero. */
int dBgCh_Gnd::DetectClsnSlot0() {
    int r = 0;
    dBgW *o;
    func_02037464(this);
    o = (dBgW *)data_020a0c80[0];
    if (o != 0) {
        if (func_02035354(this, (void *)func_020393b4(o)) == 0) {
            if (o->DetectClsn(*this) != 0) {
                int a = func_020393ac(o);
                int b = func_020393b4(o);
                dBgPi::SetCollider(0, a, (dActor_c *)b, (dBgW *)o);
                r = 1;
            }
        }
    }
    return r;
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
                Vector3 *src = &owner->mPos;
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
            Vector3 *src = &owner->mPos;
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

// @symbol _ZN9dBgCh_Lin10DetectClsnEv
bool dBgCh_Lin::DetectClsn()
{
    bool result = false;
    int i;
    dBgW *o;
    dActor_c *info;
    int m;
    char *e;
    Vector3 *base;
    int threshold;
    e = (char *)data_020a0c80[0];
    if (e != 0) {
        if (func_02035354(this, (void *)func_020393b4(e)) == 0) {
            int mask = ((dBgW *)e)->DetectClsn(*this);
            if (mask != 0) {
                dBgPi::SetCollider(0, func_020393ac(e), (dActor_c *)func_020393b4(e), (dBgW *)e);
                result = true;
            }
        }
    }

    dM3dGSph &sphere = mBoundSphere;
    base = &sphere.centre;
    threshold = (int)sphere.radius;
    for (i = 1; i < 0x18; i++) {
        o = (dBgW *)data_020a0c80[i];
        if (o == 0) continue;
        info = (dActor_c *)func_020393b4(o);
        if (func_02035354(this, info) != 0) continue;
        if (info != 0) {
            int active = (info->mFlags & 2) ? 1 : 0;
            if (active != 0) {
                Vec3 v;
                const char *position = (const char *)info + 0x5c;
                v.x = *(const s32 *)(position + 0);
                v.y = *(const s32 *)(position + 4);
                v.z = *(const s32 *)(position + 8);
                m = func_0203939c(o);
                if (m == -0x1000) {
                    int b4 = info->mClipOffsetY;
                    int b8 = info->mClipRadius;
                    v.y += b4;
                    m = b8 << 3;
                } else {
                    v.y += func_0203938c(o);
                }
                if (Vec3_Dist(base, (const Vector3 *)&v) > m + threshold)
                    continue;
            }
        }
        {
            int mask = o->DetectClsn(*this);
            if (mask != 0) {
                dBgPi::SetCollider(i, func_020393ac(o), (dActor_c *)func_020393b4(o), (dBgW *)o);
                result = true;
            }
        }
    }
    return result;
}

// @symbol func_0203859c
/* Slot-0-only line scan: shared with the full scan above, kept under its
   address name. */
extern "C" int func_0203859c(void *obj)
{
    dBgW *e = (dBgW *)data_020a0c80[0];
    int result = 0;
    if (e != 0) {
        if (func_02035354(obj, (void *)func_020393b4(e)) == 0) {
            if (e->DetectClsn(*(dBgCh_Lin *)obj) != 0) {
                int a = func_020393ac(e);
                int b = func_020393b4(e);
                ((dBgPi *)((char *)obj + 0x10))->SetCollider(0, a, (dActor_c *)b, (dBgW *)e);
                result = 1;
            }
        }
    }
    return result;
}

// @symbol func_0203842c
/* Full-table line scan without the slot-0 head: shared with the member
   scans above, kept under its address name. */
extern "C" int func_0203842c(char *self)
{
    int result = 0;
    int i;
    dBgW *o;
    dActor_c *info;
    int m;
    char *base;
    int threshold;

    char *p64 = self + 0x64;
    base = p64 + 4;
    threshold = *(int *)(p64 + 0x10);
    for (i = 1; i < 0x18; i++) {
        o = (dBgW *)data_020a0c80[i];
        if (o == 0) continue;
        info = (dActor_c *)func_020393b4(o);
        if (func_02035354(self, info) != 0) continue;
        if (info != 0) {
            int active = (info->mFlags & 2) ? 1 : 0;
            if (active != 0) {
                Vector3 v;
                const char *position = (const char *)info + 0x5c;
                v.x = *(const s32 *)(position + 0);
                v.y = *(const s32 *)(position + 4);
                v.z = *(const s32 *)(position + 8);
                m = func_0203939c(o);
                if (m == -0x1000) {
                    int b4 = info->mClipOffsetY;
                    int b8 = info->mClipRadius;
                    v.y += b4;
                    m = b8 << 3;
                } else {
                    v.y += func_0203938c(o);
                }
                if (Vec3_Dist((const Vector3 *)base, &v) > m + threshold)
                    continue;
            }
        }
        {
            int mask = o->DetectClsn(*(dBgCh_Lin *)self);
            if (mask != 0) {
                ((dBgPi *)(self + 0x10))->SetCollider(i, func_020393ac(o), (dActor_c *)func_020393b4(o), (dBgW *)o);
                result = 1;
            }
        }
    }
    return result;
}

// @symbol dBgCh_Actr_UpdateDiscreteNoLava_veneer
/* dBgCh_Actr_UpdateDiscreteNoLava_veneer @ 0x2038420 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateDiscreteNoLava (0x2037024).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *self) {
    ((dBgCh_Actr *)self)->UpdateDiscreteNoLava();
}

// @symbol func_02038414
/* func_02038414 @ 0x2038414 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateDiscreteNoLava_2 (0x2036ee4).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void func_02038414(void *self) {
    ((dBgCh_Actr *)self)->UpdateDiscreteNoLava_2();
}

// @symbol func_02038408
/* func_02038408 @ 0x2038408 (arm9) -- tail-call veneer to func_02036acc (0x2036acc). */
extern "C" void func_02038408(void) {
    func_02036acc();
}

// @symbol dBgCh_Actr_UpdateContinuous_Veneer
/* dBgCh_Actr_UpdateContinuous_Veneer @ 0x20383fc (arm9) -- tail-call veneer to dBgCh_Actr::UpdateContinuous (0x20366b4).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void *self) {
    ((dBgCh_Actr *)self)->UpdateContinuous();
}

// @symbol func_020383f0
/* func_020383f0 @ 0x20383f0 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateContinuousNoLava (0x2036318).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void func_020383f0(void *self) {
    ((dBgCh_Actr *)self)->UpdateContinuousNoLava();
}

// @symbol func_020383e4
/* func_020383e4 @ 0x20383e4 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateExtraContinous (0x20358ac).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void func_020383e4(void *self) {
    ((dBgCh_Actr *)self)->UpdateExtraContinous();
}

// @symbol func_02038324
/* Bound-to dispatch: gates on func_02038298, indexes the enabled-collider
   table with dBgPi's collider index and calls its BeforeClsn virtual method. */
extern "C" void func_02038324(void *arg, int b, int c, int d)
{
    int idx;
    dBgW *obj;
    if (((dBgPi *)arg)->HasCollider() == 0) return;
    idx = ((dBgPi *)arg)->GetColliderIndex();
    obj = (dBgW *)data_020a0c80[idx];
    if (obj == 0) return;
    if (func_020393cc(obj) == 0) return;
    if (func_02038298(arg) == 0) return;
    obj->BeforeClsn(*(dBgPi *)arg,
        (dActor_c *)func_020393b4(data_020a0c80[idx]),
        *(Vector3 *)b, (Vector3_16 *)c, (Vector3_16 *)d);
}

// @symbol func_02038298
/* Enabled-collider gate: the record must be valid, name a live table slot,
   and re-identify against its entry. Member-called: the mangled free
   spellings emit the same bl and read worse under decl agreement. */
extern "C" int func_02038298(void *arg)
{
    int idx; void *e; int a, b;
    if (((dBgPi *)arg)->IsValid() == 0) return 0;
    idx = ((dBgPi *)arg)->GetColliderIndex();
    if (idx < 0) goto r;
    if (idx < 0x18) goto work;
r:  return 0;
work:
    e = data_020a0c80[idx];
    if (e == 0) return 0;
    a = func_020393ac(e);
    b = func_020393b4(e);
    return ((dBgPi *)arg)->IsHitBy(a, (dActor_c *)b, (dBgW *)e);
}

// @symbol func_02038234
/* The free helper that ends the CLPS_BlockRef run: gates on func_02038298,
   indexes the enabled-collider table with dBgPi's collider index and
   dispatches the entry's stored callback. Calls the gate through a
   two-word function-pointer cast: the definition takes one word while
   this site passes two, and the extra register rides along. */
extern "C" void func_02038234(int a, int b)
{
    int idx;
    dBgW *entry;
    PFN fn;
    if (((int (*)(int, int))func_02038298)(a, b) == 0) return;
    idx = ((dBgPi *)a)->GetColliderIndex();
    entry = data_020a0c80[idx];
    fn = (PFN)func_020393bc((int *)entry);
    if (fn == 0) return;
    fn((int)entry, func_020393b4((int *)entry), b);
}

// @symbol _ZN13CLPS_BlockRefC1Ev
CLPS_BlockRef::CLPS_BlockRef() : ptr(0) {}

// @symbol _ZN13CLPS_BlockRefD1Ev
CLPS_BlockRef::~CLPS_BlockRef() {}

// @symbol _ZN13CLPS_BlockRefaSER10CLPS_Block
CLPS_BlockRef &CLPS_BlockRef::operator=(CLPS_Block &block)
{
    ptr = &block;
    return *this;
}

// @symbol func_020381cc
/* CLPS entry lookup: walks the collider table through a pointer-to-pointer
   argument. Written first to land last in the ROM (reverse emission). */
extern "C" {
void func_020381cc(void *r0, int i, void **out)
{
    unsigned char *e = *(unsigned char **)r0;
    if (e == 0 || *(unsigned short *)(e + 4) != 8) {
        func_02037e9c(&data_020a0c78);
        *out = &data_020a0c78;
        return;
    }
    *out = e + 8 + (i << 3);
}
}

// @symbol _ZN5dBgPiC1Ev
// @symbol _ZN5dBgPiC2Ev
dBgPi::dBgPi()
{
    Reset();
}

// @symbol _ZN5dBgPiD1Ev
// @symbol _ZN5dBgPiD2Ev
// @symbol _ZN5dBgPiD0Ev
dBgPi::~dBgPi()
{
}

void dBgPi::Reset()
{
    triangleID = 0xffff;
    colliderIdx = 0x18;
    clsnID = -1;
    owner = 0;
    collider = 0;
}

// @symbol _ZNK5dBgPi6CopyToERS_
void dBgPi::CopyTo(dBgPi &dst) const
{
    *reinterpret_cast<u64 *>(&dst.surface.clps) =
        *reinterpret_cast<const u64 *>(&surface.clps);
    dst.surface.normal.x = surface.normal.x;
    dst.surface.normal.y = surface.normal.y;
    dst.surface.normal.z = surface.normal.z;
    dst.triangleID = triangleID;
    dst.colliderIdx = colliderIdx;
    dst.clsnID = clsnID;
    dst.owner = owner;
    dst.collider = collider;
}

// @symbol _ZN5dBgPiaSERKS_
dBgPi &dBgPi::operator=(const dBgPi &other)
{
    /* CLPS is one 64-bit record. This spelling also preserves its two loads
       before either store when source and destination might overlap. */
    *reinterpret_cast<u64 *>(&surface.clps) =
        *reinterpret_cast<const u64 *>(&other.surface.clps);
    surface.normal.x = other.surface.normal.x;
    surface.normal.y = other.surface.normal.y;
    surface.normal.z = other.surface.normal.z;
    triangleID = other.triangleID;
    colliderIdx = other.colliderIdx;
    clsnID = other.clsnID;
    owner = other.owner;
    collider = other.collider;
    return *this;
}

// @symbol _ZN5dBgPi11SetColliderEiiP8dActor_cP4dBgW
void dBgPi::SetCollider(int idx, int uid, dActor_c *actor, dBgW *clsn)
{
    SetColliderIndex(idx);
    clsnID = uid;
    owner = actor;
    collider = clsn;
}

// @symbol _ZN5dBgPi9RecordHitEsP11SurfaceInfo
void dBgPi::RecordHit(s16 triID, SurfaceInfo *src)
{
    triangleID = triID;
    func_02037de8(reinterpret_cast<int *>(&surface), reinterpret_cast<int *>(src));
}

// @symbol _ZNK5dBgPi11HasColliderEv
int dBgPi::HasCollider() const
{
    return colliderIdx != 0x18;
}

// @symbol _ZNK5dBgPi7IsValidEv
int dBgPi::IsValid() const
{
    return colliderIdx != 0x18 && triangleID != 0xffff;
}

// @symbol _ZNK5dBgPi7IsHitByEiP8dActor_cP4dBgW
int dBgPi::IsHitBy(int uid, dActor_c *actor, dBgW *clsn) const
{
    return clsnID == uid && owner == actor && collider == clsn;
}

// @symbol _ZN5dBgPi16SetColliderIndexEi
void dBgPi::SetColliderIndex(int idx)
{
    colliderIdx = idx;
}

// @symbol _ZNK5dBgPi16GetColliderIndexEv
int dBgPi::GetColliderIndex() const
{
    return colliderIdx;
}

// @symbol _ZNK5dBgPi9GetClsnIDEv
u32 dBgPi::GetClsnID() const
{
    return clsnID;
}

// @symbol func_02037f44
/* The word-8 record read that ends the dBgPc run: kept as C inside
   extern "C" (probe C++OK:hoist). */
extern "C" {
int func_02037f44(int *p)
{
    return p[8];
}
}

/* The constructor is defined before the destructor so the deferred
 * emission order lands the D2,D1 group below the C1,C2 group, matching
 * the ROM's layout (mwccarm emits .text in reverse source order). */
// @symbol _ZN5dBgPcC1Ev
// @symbol _ZN5dBgPcC2Ev
dBgPc::dBgPc()
{
    surface.clps.w0 = 0xfc0;
    surface.clps.w1 = 0xff;
    surface.normal.z = 0;
    surface.normal.y = surface.normal.z;
    surface.normal.x = surface.normal.y;
}

// @symbol _ZN5dBgPcD2Ev
// @symbol _ZN5dBgPcD1Ev
dBgPc::~dBgPc()
{
}

// @symbol func_02037eb0
/* The record init that opens the dBgPc run. Local record spellings travel
 * with the TU (renamed PcAB/PcObj: the scan stand-ins below reuse Obj for
 * vtable views). struct Vector3 comes from the header chain. */
extern "C" {
void func_02037eb0(struct PcObj *o, struct PcAB ab, struct Vector3 *v)
{
    o->a = ab.a;
    o->b = ab.b;
    o->v.x = v->x;
    o->v.y = v->y;
    o->v.z = v->z;
}
}

// @symbol func_02037e9c
extern "C" {
void func_02037e9c(char *p)
{
    *(int *)(p + 0x0) = 4032;
    *(int *)(p + 0x4) = 255;
}
}

// @symbol func_02037e90
extern "C" {
int func_02037e90(int *p)
{
    return p[1] & 255;
}
}

// @symbol func_02037e84
extern "C" {
int func_02037e84(int *p)
{
    return p[0] & 31;
}
}

// @symbol SurfaceInfo_TestFlag0x20
extern "C" {
int SurfaceInfo_TestFlag0x20(int *p)
{
    return p[0] & 32;
}
}

// @symbol func_02037e68
extern "C" {
int func_02037e68(unsigned int *p)
{
    return (p[0] >> 6) & 63;
}
}

// @symbol func_02037e58
extern "C" {
int func_02037e58(unsigned int *p)
{
    return (p[0] >> 12) & 7;
}
}

// @symbol func_02037e48
extern "C" {
int func_02037e48(unsigned int *p)
{
    return (p[0] >> 15) & 15;
}
}

// @symbol func_02037e38
extern "C" {
int func_02037e38(unsigned int *p)
{
    return (p[0] >> 19) & 31;
}
}

// @symbol func_02037e2c
extern "C" {
int func_02037e2c(int *p)
{
    return p[0] & 16777216;
}
}

// @symbol func_02037e20
extern "C" {
int func_02037e20(int *p)
{
    return p[0] & 33554432;
}
}

// @symbol func_02037e14
extern "C" {
int func_02037e14(int *p)
{
    return p[0] & 67108864;
}
}

// @symbol func_02037de8
/* Five-word surface copy: one 64-bit record plus three words. Spelled with
   the u64 unit (not the C struct-assign the shard used): C++ schedules the
   struct form load/load/store while the cartridge is load/store
   interleaved, and the u64 spelling recovers it byte-identically. */
extern "C" {
void func_02037de8(int *dst, int *src)
{
    *reinterpret_cast<u64 *>(dst) = *reinterpret_cast<const u64 *>(src);
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
}
}

// @symbol _ZNK11SurfaceInfo12CopyNormalToER7Vector3
/* SurfaceInfo::CopyNormalTo(Vector3 &out) const -- copy the face normal out.
 * The normal sits at 0x08, after the 8-byte CLPS entry; see include/SurfaceInfo.h
 * for how both halves of this 0x14-byte struct are pinned. */
void SurfaceInfo::CopyNormalTo(Vector3 &out) const
{
    out.x = normal.x;
    out.y = normal.y;
    out.z = normal.z;
}

// @symbol func_02037dc4
extern "C" {
int func_02037dc4(int p)
{
    return p + 8;
}
}
