//cpp
// Sphere collision query: a dM3dGSph probe sphere (secondary base) swept
// against the KCL collider registry, with a dBgPi base record for the best
// hit and mClsnResult1/2/3 holding the floor / wall / underneath hits.
// DetectClsn and the two registry scans live in d_bg_ch_sph_crr_query.cpp.
#include "dBgCh_SphCrr.h"

// @symbol _ZN12dBgCh_SphCrrC1Ev
dBgCh_SphCrr::dBgCh_SphCrr() : mWallRejectHalfExtent(0) {}

// @symbol _ZN12dBgCh_SphCrrD1Ev
// @symbol _ZN12dBgCh_SphCrrD0Ev
// @symbol _ZThn16_N12dBgCh_SphCrrD0Ev
// @symbol _ZThn16_N12dBgCh_SphCrrD1Ev
// @symbol _ZThn56_N12dBgCh_SphCrrD0Ev
// @symbol _ZThn56_N12dBgCh_SphCrrD1Ev
dBgCh_SphCrr::~dBgCh_SphCrr()
{
}

// @symbol _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector3iP8dActor_c
void dBgCh_SphCrr::SetObjAndSphere(const Vector3 &pos, Fix12i radius, dActor_c *actor) {
    /* through the REFERENCE: a pointer-level upcast makes mwcc emit the
       null-checked MI adjustment (movs/addne), the ROM's is unconditional */
    ((dM3dGSph &)*this).Set(pos, radius);
    SetActor(actor);
    func_02037b5c();
    mMaxNormalY = 0x1000;
}

/* Reset the whole query state: bounds, every flag bit, the base record and
   all three result slots, then the floor normal (Y back to -0x1000). */
// @symbol _ZN12dBgCh_SphCrr13func_02037b5cEv
void dBgCh_SphCrr::func_02037b5c()
{
    ResetDisplacement();
    flags &= ~1;
    flags &= ~4;
    flags &= ~8;
    flags &= ~0x10;
    flags &= ~2;
    flags &= ~0x20;
    flags &= ~0x40;
    dBgPi::Reset();
    mClsnResult1.Reset();
    mClsnResult2.Reset();
    mClsnResult3.Reset();
    mFloorNormal.x = 0;
    mFloorNormal.y = -0x1000;
    mFloorNormal.z = 0;
}

/* Clear the displacement and the push-out range the scan accumulates. */
// @symbol _ZN12dBgCh_SphCrr17ResetDisplacementEv
void dBgCh_SphCrr::ResetDisplacement() {
    disp.x = disp.y = disp.z = 0;
    mPushOutMin.x = mPushOutMax.x = 0;
    mPushOutMin.y = mPushOutMax.y = 0;
    mPushOutMin.z = mPushOutMax.z = 0;
}

/* Widen the push-out range to cover a scan's lowest and highest pushes. */
// @symbol _ZN12dBgCh_SphCrr19ExpandPushOutBoundsEiiiiii
void dBgCh_SphCrr::ExpandPushOutBounds(s32 minX, s32 minY, s32 minZ, s32 maxX, s32 maxY, s32 maxZ) {
    if (mPushOutMin.x > minX)
        mPushOutMin.x = minX;
    if (mPushOutMax.x < minX)
        mPushOutMax.x = minX;
    if (mPushOutMin.x > maxX)
        mPushOutMin.x = maxX;
    if (mPushOutMax.x < maxX)
        mPushOutMax.x = maxX;
    if (mPushOutMin.y > minY)
        mPushOutMin.y = minY;
    if (mPushOutMax.y < minY)
        mPushOutMax.y = minY;
    if (mPushOutMin.y > maxY)
        mPushOutMin.y = maxY;
    if (mPushOutMax.y < maxY)
        mPushOutMax.y = maxY;
    if (mPushOutMin.z > minZ)
        mPushOutMin.z = minZ;
    if (mPushOutMax.z < minZ)
        mPushOutMax.z = minZ;
    if (mPushOutMin.z > maxZ)
        mPushOutMin.z = maxZ;
    if (mPushOutMax.z < maxZ)
        mPushOutMax.z = maxZ;
}

/* Commit the accumulated range into the displacement the actor moves by. */
// @symbol _ZN12dBgCh_SphCrr18UpdateDisplacementEv
void dBgCh_SphCrr::UpdateDisplacement() {
    disp.x = mPushOutMin.x + mPushOutMax.x;
    disp.y = mPushOutMin.y + mPushOutMax.y;
    disp.z = mPushOutMin.z + mPushOutMax.z;
}

// @symbol _ZN12dBgCh_SphCrr16GetPushOutBoundsEP7Vector3S1_
void dBgCh_SphCrr::GetPushOutBounds(Vector3 *outMin, Vector3 *outMax) {
    outMin->x = mPushOutMin.x;
    outMin->y = mPushOutMin.y;
    outMin->z = mPushOutMin.z;
    outMax->x = mPushOutMax.x;
    outMax->y = mPushOutMax.y;
    outMax->z = mPushOutMax.z;
}

/* Hit-record forwarders for the three result slots (1 = floor, 2 = wall,
   3 = underneath): one copies the surface hit, one the collider identity. */
// @symbol _ZN12dBgCh_SphCrr14RecordFloorHitEiPv
void dBgCh_SphCrr::RecordFloorHit(int triID, void *src) {
    mClsnResult1.RecordHit(triID, (SurfaceInfo *)src);
}

// @symbol _ZN12dBgCh_SphCrr16SetFloorColliderEiiP8dActor_cP4dBgW
void dBgCh_SphCrr::SetFloorCollider(int i, int clsnID, dActor_c *owner, dBgW *collider) {
    mClsnResult1.SetCollider(i, clsnID, owner, collider);
}

// @symbol _ZN12dBgCh_SphCrr13RecordWallHitEiPv
void dBgCh_SphCrr::RecordWallHit(int triID, void *src) {
    mClsnResult2.RecordHit(triID, (SurfaceInfo *)src);
}

// @symbol _ZN12dBgCh_SphCrr15SetWallColliderEiiP8dActor_cP4dBgW
void dBgCh_SphCrr::SetWallCollider(int i, int clsnID, dActor_c *owner, dBgW *collider) {
    mClsnResult2.SetCollider(i, clsnID, owner, collider);
}

// @symbol _ZN12dBgCh_SphCrr14RecordUnderHitEiPv
void dBgCh_SphCrr::RecordUnderHit(int triID, void *src) {
    mClsnResult3.RecordHit(triID, (SurfaceInfo *)src);
}

// @symbol _ZN12dBgCh_SphCrr16SetUnderColliderEiiP8dActor_cP4dBgW
void dBgCh_SphCrr::SetUnderCollider(int i, int clsnID, dActor_c *owner, dBgW *collider) {
    mClsnResult3.SetCollider(i, clsnID, owner, collider);
}

/* Save the selected floor normal; the collision scan compares Y first. */
// @symbol _ZN12dBgCh_SphCrr14SetFloorNormalEPKi
void dBgCh_SphCrr::SetFloorNormal(const s32 *normal) {
    mFloorNormal.x = normal[0];
    mFloorNormal.y = normal[1];
    mFloorNormal.z = normal[2];
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

/* Copy a whole hit record into one result slot. The CLPS is copied through
   a u64 view: a plain struct assignment interleaves its two words with the
   stores, the ROM loads both first. */
// @symbol _ZN12dBgCh_SphCrr14SetFloorResultERK5dBgPi
void dBgCh_SphCrr::SetFloorResult(const dBgPi &result) {
    *reinterpret_cast<u64 *>(&mClsnResult1.surface.clps) =
        *reinterpret_cast<const u64 *>(&result.surface.clps);
    mClsnResult1.surface.normal.x = result.surface.normal.x;
    mClsnResult1.surface.normal.y = result.surface.normal.y;
    mClsnResult1.surface.normal.z = result.surface.normal.z;
    mClsnResult1.triangleID = result.triangleID;
    mClsnResult1.colliderIdx = result.colliderIdx;
    mClsnResult1.clsnID = result.clsnID;
    mClsnResult1.owner = result.owner;
    mClsnResult1.collider = result.collider;
}

// @symbol _ZN12dBgCh_SphCrr13GetWallResultEv
dBgPi *dBgCh_SphCrr::GetWallResult()
{
    return &mClsnResult2;
}

// @symbol _ZN12dBgCh_SphCrr13SetWallResultERK5dBgPi
void dBgCh_SphCrr::SetWallResult(const dBgPi &result) {
    *reinterpret_cast<u64 *>(&mClsnResult2.surface.clps) =
        *reinterpret_cast<const u64 *>(&result.surface.clps);
    mClsnResult2.surface.normal.x = result.surface.normal.x;
    mClsnResult2.surface.normal.y = result.surface.normal.y;
    mClsnResult2.surface.normal.z = result.surface.normal.z;
    mClsnResult2.triangleID = result.triangleID;
    mClsnResult2.colliderIdx = result.colliderIdx;
    mClsnResult2.clsnID = result.clsnID;
    mClsnResult2.owner = result.owner;
    mClsnResult2.collider = result.collider;
}

// @symbol _ZN12dBgCh_SphCrr14GetUnderResultEv
dBgPi *dBgCh_SphCrr::GetUnderResult()
{
    return &mClsnResult3;
}

// @symbol _ZN12dBgCh_SphCrr14SetUnderResultERK5dBgPi
void dBgCh_SphCrr::SetUnderResult(const dBgPi &result) {
    *reinterpret_cast<u64 *>(&mClsnResult3.surface.clps) =
        *reinterpret_cast<const u64 *>(&result.surface.clps);
    mClsnResult3.surface.normal.x = result.surface.normal.x;
    mClsnResult3.surface.normal.y = result.surface.normal.y;
    mClsnResult3.surface.normal.z = result.surface.normal.z;
    mClsnResult3.triangleID = result.triangleID;
    mClsnResult3.colliderIdx = result.colliderIdx;
    mClsnResult3.clsnID = result.clsnID;
    mClsnResult3.owner = result.owner;
    mClsnResult3.collider = result.collider;
}
