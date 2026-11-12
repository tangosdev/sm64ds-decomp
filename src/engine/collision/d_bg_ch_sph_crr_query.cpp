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
 * name. SurfaceInfo::CopyFrom copies the five-word surface with a u64 so the
 * loads and stores interleave.
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
#include "math/CalcLib.h"

extern "C" {
int dBgS_ChkPolySafe(void *arg);
extern CLPS data_020a0c78;
}

/* Ground and line scans use the real dBgW virtual interface and dActor_c
   clip-volume fields, as the sphere scans do below. PFN retains its pending
   stored-callback reconstruction. Position reads go through a
   `const Vector3 *` to owner->mPos: direct owner->mPos.x/y/z accesses remove
   one address-setup instruction from each of three scans. */

int dBgCh_Gnd::DetectClsn()
{
    int ret = 0;
    ResetResult();
    dBgW *o = data_020a0c80[0];
    if (o != 0 && !IsOwnedBy(o->GetActor()) && o->DetectClsn(*this) != 0) {
        dBgPi::SetCollider(0, o->GetActorUniqueID(), o->GetActor(), o);
        ret = 1;
    }
    int flag = 1;
    int one = 1;
    Vector3 ownerPos;
    for (int i = 1; i < 24; i++) {
        dBgW *obj = data_020a0c80[i];
        if (obj == 0) continue;
        dActor_c *owner = obj->GetActor();
        if (IsOwnedBy(owner))
            continue;
        if (owner != 0 && ((owner->mFlags & 2) ? flag : 0) != 0) {
            const Vector3 *position = &owner->mPos;
            ownerPos.x = position->x;
            ownerPos.y = position->y;
            ownerPos.z = position->z;
            int clipDist = obj->GetClipRadius();
            if (clipDist == -0x1000) {
                int clipRadius = owner->mClipRadius;
                ownerPos.y = ownerPos.y + owner->mClipOffsetY;
                clipDist = clipRadius << 3;
            } else {
                ownerPos.y = ownerPos.y + obj->GetClipOffsetY();
            }
            Vector3 *selfpos = (Vector3 *)((int)&this->pos);
            if (selfpos->y < ownerPos.y - clipDist)
                continue;
            if (Vec3_HorzDist(selfpos, &ownerPos) > clipDist)
                continue;
        }
        if (obj->DetectClsn(*this) != 0) {
            dBgPi::SetCollider(i, obj->GetActorUniqueID(), obj->GetActor(), obj);
            ret = one;
        }
    }
    return ret;
}

/* Inferred name: reset the ground query and scan only collider slot zero. */
int dBgCh_Gnd::DetectClsnSlot0() {
    int r = 0;
    dBgW *o;
    ResetResult();
    o = data_020a0c80[0];
    if (o != 0) {
        if (!IsOwnedBy(o->GetActor())) {
            if (o->DetectClsn(*this) != 0) {
                int a = o->GetActorUniqueID();
                dActor_c *owner = o->GetActor();
                dBgPi::SetCollider(0, a, owner, o);
                r = 1;
            }
        }
    }
    return r;
}

int dBgCh_SphCrr::DetectClsn()
{
    int result = 0;
    int hits;

    /* slot 0 first, then the rest of the table; e is reused for the sphere
       centre below -- the one local keeps the compiler's register shape */
    char *e = (char *)data_020a0c80[0];
    if (e != 0) {
        if (!IsOwnedBy(((dBgW *)e)->GetActor())) {
            hits = ((dBgW *)e)->DetectClsn(*this);
            if (hits != 0) {
                dBgPi::SetCollider(0, ((dBgW *)e)->GetActorUniqueID(), ((dBgW *)e)->GetActor(),
                                   (dBgW *)e);
                if (hits & 1)
                    SetFloorCollider(0, ((dBgW *)e)->GetActorUniqueID(), ((dBgW *)e)->GetActor(),
                                     (dBgW *)e);
                if (hits & 2)
                    SetWallCollider(0, ((dBgW *)e)->GetActorUniqueID(), ((dBgW *)e)->GetActor(),
                                    (dBgW *)e);
                if (hits & 4)
                    SetUnderCollider(0, ((dBgW *)e)->GetActorUniqueID(), ((dBgW *)e)->GetActor(),
                                     (dBgW *)e);
                result = 1;
            }
        }
    }

    e = (char *)&((dM3dGSph &)*this).centre;
    int reach = ((dM3dGSph &)*this).radius + 0x1000;
    for (int i = 1; i < 0x18; i++) {
        dBgW *o = data_020a0c80[i];
        if (o == 0) continue;
        dActor_c *owner = o->GetActor();
        if (IsOwnedBy(owner))
            continue;
        if (owner != 0) {
            int active = (owner->mFlags & 2) ? 1 : 0;
            if (active != 0) {
                Vector3 ownerPos;
                Vector3 *src = &owner->mPos;
                ownerPos.x = src->x;
                ownerPos.y = src->y;
                ownerPos.z = src->z;
                hits = o->GetClipRadius();
                if (hits == -0x1000) {
                    int clipOffsetY = owner->mClipOffsetY;
                    int clipRadius = owner->mClipRadius;
                    ownerPos.y += clipOffsetY;
                    hits = clipRadius << 3;
                } else {
                    ownerPos.y += o->GetClipOffsetY();
                }
                if (Vec3_Dist((const Vector3 *)e, &ownerPos) > hits + reach)
                    continue;
            }
        }
        /* the scoped temporary keeps the ROM's register shape */
        {
            int t = o->DetectClsn(*this);
            hits = t;
        }
        if (hits != 0) {
            dBgPi::SetCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
            if (hits & 1)
                SetFloorCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
            if (hits & 2)
                SetWallCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
            if (hits & 4)
                SetUnderCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
            result = 1;
        }
    }
    this->UpdateDisplacement();
    return result;
}

/* DetectClsn against collider slot 0 only: the stage mesh, which
   dScStage_c::LoadClsnAndObjects registers first and without an owner.
   Inferred name, after dBgCh_Gnd::DetectClsnSlot0. */
int dBgCh_SphCrr::DetectClsnSlot0() {
    dBgW *o = data_020a0c80[0];
    int result = 0;
    int hits;
    if (o != 0 && !IsOwnedBy(o->GetActor()) && (hits = o->DetectClsn(*this)) != 0) {
        dBgPi::SetCollider(0, o->GetActorUniqueID(), o->GetActor(), o);
        if (hits & 1)
            SetFloorCollider(0, o->GetActorUniqueID(), o->GetActor(), o);
        if (hits & 2)
            SetWallCollider(0, o->GetActorUniqueID(), o->GetActor(), o);
        if (hits & 4)
            SetUnderCollider(0, o->GetActorUniqueID(), o->GetActor(), o);
        result = 1;
    }
    this->UpdateDisplacement();
    return result;
}

/* DetectClsn without its slot-0 head: the actor-owned colliders in slots
   1..0x17, never the stage mesh. Inferred name. */
int dBgCh_SphCrr::DetectClsnExceptSlot0() {
    int reach = ((dM3dGSph &)*this).radius + 0x1000;
    int result = 0;
    int zero = 0;
    int one = 1;
    int i;
    for (i = 1; i < 0x18; i++) {
        dBgW *o = data_020a0c80[i];
        if (o == 0)
            continue;
        dActor_c *owner = o->GetActor();
        if (IsOwnedBy(owner))
            continue;
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
            clipDist = o->GetClipRadius();
            if (clipDist == -0x1000) {
                int clipRadius = owner->mClipRadius;
                ownerPos.y = ownerPos.y + owner->mClipOffsetY;
                clipDist = clipRadius << 3;
            } else {
                ownerPos.y = ownerPos.y + o->GetClipOffsetY();
            }
            if (Vec3_Dist((const Vector3 *)pos, &ownerPos) > clipDist + reach)
                continue;
        }
        int hits = o->DetectClsn(*this);
        if (hits == 0)
            continue;
        dBgPi::SetCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
        if (hits & 1)
            SetFloorCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
        if (hits & 2)
            SetWallCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
        if (hits & 4)
            SetUnderCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
        result = one;
    }
    this->UpdateDisplacement();
    return result;
}

bool dBgCh_Lin::DetectClsn()
{
    bool result = false;
    int i;
    dBgW *o;
    dActor_c *owner;
    int clipDist;
    char *e;
    Vector3 *base;
    int reach;
    e = (char *)data_020a0c80[0];
    if (e != 0) {
        if (!IsOwnedBy(((dBgW *)e)->GetActor())) {
            int mask = ((dBgW *)e)->DetectClsn(*this);
            if (mask != 0) {
                dBgPi::SetCollider(0, ((dBgW *)e)->GetActorUniqueID(), ((dBgW *)e)->GetActor(),
                                   (dBgW *)e);
                result = true;
            }
        }
    }

    dM3dGSph &sphere = mBoundSphere;
    base = &sphere.centre;
    reach = (int)sphere.radius;
    for (i = 1; i < 0x18; i++) {
        o = data_020a0c80[i];
        if (o == 0) continue;
        owner = o->GetActor();
        if (IsOwnedBy(owner))
            continue;
        if (owner != 0) {
            int active = (owner->mFlags & 2) ? 1 : 0;
            if (active != 0) {
                Vector3 ownerPos;
                const Vector3 *position = &owner->mPos;
                ownerPos.x = position->x;
                ownerPos.y = position->y;
                ownerPos.z = position->z;
                clipDist = o->GetClipRadius();
                if (clipDist == -0x1000) {
                    int clipOffsetY = owner->mClipOffsetY;
                    int clipRadius = owner->mClipRadius;
                    ownerPos.y += clipOffsetY;
                    clipDist = clipRadius << 3;
                } else {
                    ownerPos.y += o->GetClipOffsetY();
                }
                if (Vec3_Dist(base, &ownerPos) > clipDist + reach)
                    continue;
            }
        }
        {
            int mask = o->DetectClsn(*this);
            if (mask != 0) {
                dBgPi::SetCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
                result = true;
            }
        }
    }
    return result;
}

/* The line scan against collider slot 0 only, the stage mesh; the
   dBgCh_SphCrr and dBgCh_Gnd scans have the same split. Inferred name. */
int dBgCh_Lin::DetectClsnSlot0() {
    dBgW *e = data_020a0c80[0];
    int result = 0;
    if (e != 0) {
        if (!IsOwnedBy(e->GetActor())) {
            if (e->DetectClsn(*this) != 0) {
                int a = e->GetActorUniqueID();
                dActor_c *owner = e->GetActor();
                dBgPi::SetCollider(0, a, owner, e);
                result = 1;
            }
        }
    }
    return result;
}

/* DetectClsn without its slot-0 head: the actor-owned colliders in slots
   1..0x17 only. Inferred name. */
int dBgCh_Lin::DetectClsnExceptSlot0() {
    int result = 0;
    int i;
    dBgW *o;
    dActor_c *owner;
    int clipDist;
    Vector3 *base;
    int reach;

    dM3dGSph &sphere = mBoundSphere;
    base = &sphere.centre;
    reach = sphere.radius;
    for (i = 1; i < 0x18; i++) {
        o = data_020a0c80[i];
        if (o == 0)
            continue;
        owner = o->GetActor();
        if (IsOwnedBy(owner))
            continue;
        if (owner != 0) {
            int active = (owner->mFlags & 2) ? 1 : 0;
            if (active != 0) {
                Vector3 ownerPos;
                const Vector3 *position = &owner->mPos;
                ownerPos.x = position->x;
                ownerPos.y = position->y;
                ownerPos.z = position->z;
                clipDist = o->GetClipRadius();
                if (clipDist == -0x1000) {
                    int clipOffsetY = owner->mClipOffsetY;
                    int clipRadius = owner->mClipRadius;
                    ownerPos.y += clipOffsetY;
                    clipDist = clipRadius << 3;
                } else {
                    ownerPos.y += o->GetClipOffsetY();
                }
                if (Vec3_Dist(base, &ownerPos) > clipDist + reach)
                    continue;
            }
        }
        {
            int mask = o->DetectClsn(*this);
            if (mask != 0) {
                dBgPi::SetCollider(i, o->GetActorUniqueID(), o->GetActor(), o);
                result = 1;
            }
        }
    }
    return result;
}

/* dBgCh_Actr_UpdateDiscreteNoLava_veneer @ 0x2038420 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateDiscreteNoLava (0x2037024).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *self) {
    self->UpdateDiscreteNoLava();
}

/* func_02038414 @ 0x2038414 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateDiscreteNoLava_2 (0x2036ee4).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void func_02038414(void *self) {
    ((dBgCh_Actr *)self)->UpdateDiscreteNoLava_2();
}

/* func_02038408 @ 0x2038408 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateContinuousExceptSlot0 (0x2036acc).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void func_02038408(dBgCh_Actr *self) {
    self->UpdateContinuousExceptSlot0();
}

/* dBgCh_Actr_UpdateContinuous_Veneer @ 0x20383fc (arm9) -- tail-call veneer to dBgCh_Actr::UpdateContinuous (0x20366b4).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *self) {
    self->UpdateContinuous();
}

/* func_020383f0 @ 0x20383f0 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateContinuousNoLava (0x2036318).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void func_020383f0(dBgCh_Actr *self) {
    self->UpdateContinuousNoLava();
}

/* func_020383e4 @ 0x20383e4 (arm9) -- tail-call veneer to dBgCh_Actr::UpdateExtraContinous (0x20358ac).
 * r0 (the dBgCh_Actr) passes straight through to the member. */
extern "C" void func_020383e4(dBgCh_Actr *self) {
    self->UpdateExtraContinous();
}

/* Bound-to dispatch: gates on dBgS_ChkPolySafe, indexes the enabled-collider
   table with dBgPi's collider index and calls its BeforeClsn virtual method.
   Named after TWW's dBgS::MoveBgCrrPos, which runs the same guards and calls
   the collider's position correction; the dBgS_ prefix is coined (there is
   no dBgS object here, the table is a global). */
extern "C" void dBgS_MoveBgCrrPos(void *arg, int b, int c, int d) {
    dBgPi *hit = (dBgPi *)arg;
    int idx;
    dBgW *obj;
    if (hit->HasCollider() == 0)
        return;
    idx = hit->GetColliderIndex();
    obj = data_020a0c80[idx];
    if (obj == 0)
        return;
    if (obj->GetBeforeClsnCallback() == 0)
        return;
    if (dBgS_ChkPolySafe(hit) == 0)
        return;
    obj->BeforeClsn(*hit, data_020a0c80[idx]->GetActor(), *(Vector3 *)b, (Vector3_16 *)c,
                    (Vector3_16 *)d);
}

/* Enabled-collider gate: the record must be valid, name a live table slot,
   and re-identify against its entry. Named after TWW's cBgS::ChkPolySafe,
   same checks in the same order; the dBgS_ prefix is coined. Member-called: the mangled free
   spellings emit the same bl and read worse under decl agreement. */
extern "C" int dBgS_ChkPolySafe(void *arg) {
    dBgPi *hit = (dBgPi *)arg;
    int idx;
    dBgW *collider;
    int clsnID;
    dActor_c *owner;
    if (hit->IsValid() == 0)
        return 0;
    idx = hit->GetColliderIndex();
    if (idx < 0 || idx >= 0x18)
        return 0;
    collider = data_020a0c80[idx];
    if (collider == 0)
        return 0;
    clsnID = collider->GetActorUniqueID();
    owner = collider->GetActor();
    return hit->IsHitBy(clsnID, owner, collider);
}

/* The free helper that ends the CLPS_BlockRef run: gates on dBgS_ChkPolySafe,
   indexes the enabled-collider table with dBgPi's collider index and
   dispatches the entry's stored callback. Named after
   TWW's dBgS::RideCallBack (the stored callback is its ride callback); the
   dBgS_ prefix is coined. */
extern "C" void dBgS_RideCallBack(int a, int b) {
    int idx;
    dBgW *entry;
    StoodOnCallback fn;
    if (dBgS_ChkPolySafe((void *)a) == 0)
        return;
    idx = ((dBgPi *)a)->GetColliderIndex();
    entry = data_020a0c80[idx];
    fn = entry->GetStoodOnCallback();
    if (fn == 0) return;
    fn(entry, entry->GetActor(), (dActor_c *)b);
}

CLPS_BlockRef::CLPS_BlockRef() : ptr(0) {}

CLPS_BlockRef::~CLPS_BlockRef() {}

CLPS_BlockRef &CLPS_BlockRef::operator=(CLPS_Block &block)
{
    ptr = &block;
    return *this;
}

/* CLPS entry lookup: hands back entry idx of the block's 8-byte-entry
   table, or the default entry data_020a0c78 (reset by CLPS::SetDefault) when
   the block is missing or its entries are not 8 bytes. Written first to
   land last in the ROM (reverse emission). */
extern "C" void func_020381cc(void *block, int idx, void **out) {
    unsigned char *table = *(unsigned char **)block;
    if (table == 0 || *(unsigned short *)(table + 4) != 8) {
        data_020a0c78.SetDefault();
        *out = &data_020a0c78;
        return;
    }
    *out = table + 8 + (idx << 3);
}

dBgPi::dBgPi()
{
    Reset();
}

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

void dBgPi::SetCollider(int idx, int uid, dActor_c *actor, dBgW *clsn)
{
    SetColliderIndex(idx);
    clsnID = uid;
    owner = actor;
    collider = clsn;
}

void dBgPi::RecordHit(int triID, SurfaceInfo *src) {
    triangleID = triID;
    surface.CopyFrom(*src);
}

int dBgPi::HasCollider() const
{
    return colliderIdx != 0x18;
}

int dBgPi::IsValid() const
{
    return colliderIdx != 0x18 && triangleID != 0xffff;
}

int dBgPi::IsHitBy(int uid, dActor_c *actor, dBgW *clsn) const
{
    return clsnID == uid && owner == actor && collider == clsn;
}

void dBgPi::SetColliderIndex(int idx)
{
    colliderIdx = idx;
}

int dBgPi::GetColliderIndex() const
{
    return colliderIdx;
}

u32 dBgPi::GetClsnID() const
{
    return clsnID;
}

/* The owner of the collider that produced this result; it ends the dBgPi
   run. d_a_mky.cpp is its caller. */
dActor_c *dBgPi::GetOwner() const {
    return owner;
}

/* The constructor is defined before the destructor so the deferred
 * emission order lands the D2,D1 group below the C1,C2 group, matching
 * the ROM's layout (mwccarm emits .text in reverse source order). */
dBgPc::dBgPc()
{
    surface.clps.w0 = 0xfc0;
    surface.clps.w1 = 0xff;
    surface.normal.z = 0;
    surface.normal.y = surface.normal.z;
    surface.normal.x = surface.normal.y;
}

dBgPc::~dBgPc()
{
}

/* Fill a SurfaceInfo from a CLPS entry, passed by value as its two words,
   and a face normal. Opens the dBgPc run. */
extern "C" void func_02037eb0(SurfaceInfo *info, CLPS clps, Vector3 *normal) {
    info->clps.w0 = clps.w0;
    info->clps.w1 = clps.w1;
    info->normal.x = normal->x;
    info->normal.y = normal->y;
    info->normal.z = normal->z;
}

/* The default CLPS entry: the same two words dBgPc's constructor stores.
   Coined name. */
void CLPS::SetDefault() {
    w0 = 0xfc0;
    w1 = 0xff;
}

int CLPS::GetPathID() const {
    return w1 & 0xff;
}

int CLPS::GetSoundType() const {
    return w0 & 0x1f;
}

extern "C" int SurfaceInfo_TestFlag0x20(int *p) {
    return p[0] & 0x20;
}

int CLPS::GetViewID() const {
    return (w0 >> 6) & 0x3f;
}

int CLPS::GetFloorClass() const {
    return (w0 >> 12) & 0x7;
}

int CLPS::GetCameraBehavior() const {
    return (w0 >> 15) & 0xf;
}

int CLPS::GetSurfaceType() const {
    return (w0 >> 19) & 0x1f;
}

int CLPS::IsCameraPassThrough() const {
    return w0 & 0x1000000;
}

extern "C" int func_02037e20(int *p) {
    return p[0] & 0x2000000;
}

int CLPS::IsCameraOnly() const {
    return w0 & 0x4000000;
}

/* Five-word surface copy: one 64-bit record plus three words. Spelled with
   the u64 unit (not the C struct-assign the shard used): C++ schedules the
   struct form load/load/store while the cartridge is load/store
   interleaved, and the u64 spelling recovers it byte-identically. Coined
   name; dBgPi::RecordHit is its caller. */
void SurfaceInfo::CopyFrom(const SurfaceInfo &src) {
    *reinterpret_cast<u64 *>(&clps) = *reinterpret_cast<const u64 *>(&src.clps);
    normal.x = src.normal.x;
    normal.y = src.normal.y;
    normal.z = src.normal.z;
}

/* SurfaceInfo::CopyNormalTo(Vector3 &out) const -- copy the face normal out.
 * The normal sits at 0x08, after the 8-byte CLPS entry; see include/SurfaceInfo.h
 * for how both halves of this 0x14-byte struct are pinned. */
void SurfaceInfo::CopyNormalTo(Vector3 &out) const
{
    out.x = normal.x;
    out.y = normal.y;
    out.z = normal.z;
}

extern "C" int func_02037dc4(int p) {
    return p + 8;
}
