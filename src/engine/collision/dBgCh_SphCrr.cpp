//cpp
// Sphere collision query: a dM3dGSph probe sphere (secondary base) swept
// against the KCL collider registry, with a dBgPi base record for the best
// hit and mClsnResult1/2/3 holding the floor / wall / underneath hits.
// DetectClsn and the two registry scans live in dBgCh_SphCrr_query.cpp.
//
// comment leftovers:
//   - SetObjAndSphere is defined by its mangled name: the real signature
//     carries Fix12<int> by value and the member form changes codegen under
//     2004/b56. The header declaration is the real one; callers use it.
//   - func_020353b0 stays a free extern: the shared dBgCh-side bind helper is
//     owned by its own shard and called by the Gnd/Lin query TUs as well.
//   - unk_0fc/unk_100/unk_104 stay unnamed: all that is evidenced is a score
//     at +0x100 selecting a three-word payload starting at +0xfc (see the
//     class header).
#include "dBgCh_SphCrr.h"

extern "C" {
void func_020353b0(char *c, int *p);    /* local extern: shared bind helper,
   writes the bound actor + its uniqueID into the query's dBgCh tail */
/* local extern: dBgPi::RecordHit by its mangled name -- the forwarders take
   int triID byte-required; the member's s16 parameter emits sxth here. */
void _ZN5dBgPi9RecordHitEsP11SurfaceInfo(void *res, int triID, void *info);
}

// @symbol _ZN12dBgCh_SphCrrC1Ev
dBgCh_SphCrr::dBgCh_SphCrr() : unk_0ec(0) {}

// @symbol _ZN12dBgCh_SphCrrD1Ev
// @symbol _ZN12dBgCh_SphCrrD0Ev
// @symbol _ZThn16_N12dBgCh_SphCrrD0Ev
// @symbol _ZThn16_N12dBgCh_SphCrrD1Ev
// @symbol _ZThn56_N12dBgCh_SphCrrD0Ev
// @symbol _ZThn56_N12dBgCh_SphCrrD1Ev
dBgCh_SphCrr::~dBgCh_SphCrr()
{
}

extern "C" void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
    dBgCh_SphCrr *self, const Vector3 *pos, int radius, dActor_c *actor)
{
    /* through the REFERENCE: a pointer-level upcast makes mwcc emit the
       null-checked MI adjustment (movs/addne), the ROM's is unconditional */
    ((dM3dGSph &)*self).Set(*pos, radius);
    func_020353b0((char *)self, (int *)actor);
    self->func_02037b5c();
    self->mScale = 0x1000;
}

/* Reset the whole query state: bounds, every flag bit, the base record and
   all three result slots, then the +0xfc payload (score back to -0x1000). */
// @symbol _ZN12dBgCh_SphCrr13func_02037b5cEv
void dBgCh_SphCrr::func_02037b5c()
{
    func_02037b1c();
    flags &= ~1;
    flags &= ~4;
    flags &= ~8;
    flags &= ~0x10;
    flags &= ~2;
    flags &= ~0x20;
    flags &= ~0x40;
    ((dBgPi &)*this).Reset();
    mClsnResult1.Reset();
    mClsnResult2.Reset();
    mClsnResult3.Reset();
    unk_0fc = 0;
    unk_100 = -0x1000;
    unk_104 = 0;
}

/* Clear the displacement and the broad-phase box the scan accumulates. */
// @symbol _ZN12dBgCh_SphCrr13func_02037b1cEv
void dBgCh_SphCrr::func_02037b1c()
{
    disp.x = disp.y = disp.z = 0;
    aabbMin.x = aabbMax.x = 0;
    aabbMin.y = aabbMax.y = 0;
    aabbMin.z = aabbMax.z = 0;
}

/* Grow the broad-phase box so it covers both of a hit's corner points. */
// @symbol _ZN12dBgCh_SphCrr13func_02037a6cEiiiiii
void dBgCh_SphCrr::func_02037a6c(s32 minX, s32 minY, s32 minZ, s32 maxX, s32 maxY, s32 maxZ)
{
    if (aabbMin.x > minX) aabbMin.x = minX;
    if (aabbMax.x < minX) aabbMax.x = minX;
    if (aabbMin.x > maxX) aabbMin.x = maxX;
    if (aabbMax.x < maxX) aabbMax.x = maxX;
    if (aabbMin.y > minY) aabbMin.y = minY;
    if (aabbMax.y < minY) aabbMax.y = minY;
    if (aabbMin.y > maxY) aabbMin.y = maxY;
    if (aabbMax.y < maxY) aabbMax.y = maxY;
    if (aabbMin.z > minZ) aabbMin.z = minZ;
    if (aabbMax.z < minZ) aabbMax.z = minZ;
    if (aabbMin.z > maxZ) aabbMin.z = maxZ;
    if (aabbMax.z < maxZ) aabbMax.z = maxZ;
}

/* Commit the accumulated box into the displacement the actor moves by. */
// @symbol _ZN12dBgCh_SphCrr13func_02037a38Ev
void dBgCh_SphCrr::func_02037a38()
{
    disp.x = aabbMin.x + aabbMax.x;
    disp.y = aabbMin.y + aabbMax.y;
    disp.z = aabbMin.z + aabbMax.z;
}

// @symbol _ZN12dBgCh_SphCrr13func_02037a04EP7Vector3S1_
void dBgCh_SphCrr::func_02037a04(Vector3 *outMin, Vector3 *outMax)
{
    outMin->x = aabbMin.x;
    outMin->y = aabbMin.y;
    outMin->z = aabbMin.z;
    outMax->x = aabbMax.x;
    outMax->y = aabbMax.y;
    outMax->z = aabbMax.z;
}

/* Hit-record forwarders for the three result slots (1 = floor, 2 = wall,
   3 = underneath): one copies the surface hit, one the collider identity. */
// @symbol _ZN12dBgCh_SphCrr13func_020379f4EiPv
void dBgCh_SphCrr::func_020379f4(int triID, void *src)
{
    _ZN5dBgPi9RecordHitEsP11SurfaceInfo(&mClsnResult1, triID, src);
}

// @symbol _ZN12dBgCh_SphCrr13func_020379d0EiiP8dActor_cP4dBgW
void dBgCh_SphCrr::func_020379d0(int i, int clsnID, dActor_c *owner, dBgW *collider)
{
    mClsnResult1.SetCollider(i, clsnID, owner, collider);
}

// @symbol _ZN12dBgCh_SphCrr13func_020379c0EiPv
void dBgCh_SphCrr::func_020379c0(int triID, void *src)
{
    _ZN5dBgPi9RecordHitEsP11SurfaceInfo(&mClsnResult2, triID, src);
}

// @symbol _ZN12dBgCh_SphCrr13func_0203799cEiiP8dActor_cP4dBgW
void dBgCh_SphCrr::func_0203799c(int i, int clsnID, dActor_c *owner, dBgW *collider)
{
    mClsnResult2.SetCollider(i, clsnID, owner, collider);
}

// @symbol _ZN12dBgCh_SphCrr13func_0203798cEiPv
void dBgCh_SphCrr::func_0203798c(int triID, void *src)
{
    _ZN5dBgPi9RecordHitEsP11SurfaceInfo(&mClsnResult3, triID, src);
}

// @symbol _ZN12dBgCh_SphCrr13func_02037968EiiP8dActor_cP4dBgW
void dBgCh_SphCrr::func_02037968(int i, int clsnID, dActor_c *owner, dBgW *collider)
{
    mClsnResult3.SetCollider(i, clsnID, owner, collider);
}

/* Copy the three-word payload the winning scan selected at +0xfc. */
// @symbol _ZN12dBgCh_SphCrr13func_0203794cEPKi
void dBgCh_SphCrr::func_0203794c(const s32 *payload)
{
    unk_0fc = payload[0];
    unk_100 = payload[1];
    unk_104 = payload[2];
}

/* Merge another query's flag byte, dropping its hit-result bits (0x1c). */
// @symbol _ZN12dBgCh_SphCrr13func_02037940Eh
void dBgCh_SphCrr::func_02037940(u8 flags_)
{
    flags = flags_ & ~0x1c;
}

// @symbol _ZN12dBgCh_SphCrr14GetFloorResultEv
dBgPi *dBgCh_SphCrr::GetFloorResult()
{
    return &mClsnResult1;
}

// @symbol _ZN12dBgCh_SphCrr14SetFloorResultERK5dBgPi
void dBgCh_SphCrr::SetFloorResult(const dBgPi &src_)
{
    *reinterpret_cast<u64 *>(&mClsnResult1.surface.clps) =
        *reinterpret_cast<const u64 *>(&src_.surface.clps);
    mClsnResult1.surface.normal.x = src_.surface.normal.x;
    mClsnResult1.surface.normal.y = src_.surface.normal.y;
    mClsnResult1.surface.normal.z = src_.surface.normal.z;
    mClsnResult1.triangleID = src_.triangleID;
    mClsnResult1.colliderIdx = src_.colliderIdx;
    mClsnResult1.clsnID = src_.clsnID;
    mClsnResult1.owner = src_.owner;
    mClsnResult1.collider = src_.collider;
}

// @symbol _ZN12dBgCh_SphCrr13GetWallResultEv
dBgPi *dBgCh_SphCrr::GetWallResult()
{
    return &mClsnResult2;
}

// @symbol _ZN12dBgCh_SphCrr13SetWallResultERK5dBgPi
void dBgCh_SphCrr::SetWallResult(const dBgPi &src_)
{
    *reinterpret_cast<u64 *>(&mClsnResult2.surface.clps) =
        *reinterpret_cast<const u64 *>(&src_.surface.clps);
    mClsnResult2.surface.normal.x = src_.surface.normal.x;
    mClsnResult2.surface.normal.y = src_.surface.normal.y;
    mClsnResult2.surface.normal.z = src_.surface.normal.z;
    mClsnResult2.triangleID = src_.triangleID;
    mClsnResult2.colliderIdx = src_.colliderIdx;
    mClsnResult2.clsnID = src_.clsnID;
    mClsnResult2.owner = src_.owner;
    mClsnResult2.collider = src_.collider;
}

// @symbol _ZN12dBgCh_SphCrr14GetUnderResultEv
dBgPi *dBgCh_SphCrr::GetUnderResult()
{
    return &mClsnResult3;
}

// @symbol _ZN12dBgCh_SphCrr14SetUnderResultERK5dBgPi
void dBgCh_SphCrr::SetUnderResult(const dBgPi &src_)
{
    *reinterpret_cast<u64 *>(&mClsnResult3.surface.clps) =
        *reinterpret_cast<const u64 *>(&src_.surface.clps);
    mClsnResult3.surface.normal.x = src_.surface.normal.x;
    mClsnResult3.surface.normal.y = src_.surface.normal.y;
    mClsnResult3.surface.normal.z = src_.surface.normal.z;
    mClsnResult3.triangleID = src_.triangleID;
    mClsnResult3.colliderIdx = src_.colliderIdx;
    mClsnResult3.clsnID = src_.clsnID;
    mClsnResult3.owner = src_.owner;
    mClsnResult3.collider = src_.collider;
}
