//cpp
/* The callback setters and CLPS water/toxic tests still use free-function
 * interfaces. The enabled-collider table is shared with the query scans. */
/* dBgW: root of the KCL collider family. Type name 0x02099370, typeinfo
 * 0x02099378, vtable 0x02099388, 13 slots. This TU also defines
 * dScStage_c::ResetMeshColliders and dBgCh::ShouldPassThroughImpl.
 */
#include "dBgW.h"
#include "dBgW_Kc.h"
#include "dBgCh.h"
#include "CLPS.h"
#include "dScStage_c.h"
#include "dActor_c.h"
#include "decl_common.h"

extern "C" {
/* local extern: no header declares the CLPS water test (CLPS.h only
   documents it); d_bg_ch_sph_crr_query.cpp defines it with this
   signature. */
extern int SurfaceInfo_TestFlag0x20(int *clpsWords);
}


/* Stores dBgW_Kc::depthDirMinDot (+0x44), the sphere query's face-dot threshold. */
void dBgW_Kc::SetDepthDirMinDot(Fix12i value) {
    depthDirMinDot = value;
}


/* Stores dBgW_Kc::edgeToleranceShift (+0x48), clamped at 0. */
void dBgW_Kc::SetEdgeToleranceShift(int value) {
    if (value < 0)
        value = 0;
    edgeToleranceShift = value;
}

dBgW::dBgW() {
    Reset();
}

dBgW::~dBgW() {}

void dBgW::Reset() {
    owner = 0;
    ownerUniqueID = -1;
    slotIdx = 0x18;
    beforeClsnCallback = 0;
    stoodOnCallback = 0;
    clipRadius = -0x1000;
    clipOffsetY = 0;
}

void dBgW::Virtual08() {}


/* Binds the collider to its owning actor, or clears the binding. */
void dBgW::SetActor(dActor_c *actor) {
    if (actor == 0) {
        owner = 0;
        ownerUniqueID = -1;
    } else {
        owner = actor;
        ownerUniqueID = actor->uniqueID;
    }
}

bool dBgCh::ShouldPassThroughImpl(void *unused, const CLPS &surfaceProperties, const dBgCh &query,
                                  bool checkType14) {
    /* Each special surface asks whether the query detects its kind, through
       dBgCh::mDetectFlags: a water surface (CLPS 0x20) needs the water bit
       (2), CLPS bit 0x02000000 needs the toxic bit (8). Surface types 0x11
       and 0x14 run the other way: the query passes through them when it is
       ignoring them. A surface no rule covers stops only queries that detect
       ordinary ground (bit 1). Camera mode skips camera-pass-through
       surfaces and enables camera-only surfaces. */
    int *clpsWords = (int *)&surfaceProperties;
    bool ruleApplies = false;

    if (SurfaceInfo_TestFlag0x20(clpsWords)) {
        ruleApplies = true;
        if (query.IsDetectingWater() == 0)
            return true;
    }
    if (func_02037e20(clpsWords)) {
        ruleApplies = true;
        if (query.IsDetectingToxic() == 0)
            return true;
    }
    if (surfaceProperties.IsCameraPassThrough()) {
        ruleApplies = true;
        if (query.IsCameraMode() != 0)
            return true;
    }
    if (surfaceProperties.IsCameraOnly()) {
        ruleApplies = true;
        if (query.IsCameraMode() == 0)
            return true;
    }
    if (surfaceProperties.GetSurfaceType() == 0x11) {
        ruleApplies = true;
        if (query.IsIgnoringVanishSurfaces())
            return true;
    }
    if (checkType14 && surfaceProperties.GetSurfaceType() == 0x14) {
        ruleApplies = true;
        if (query.IsIgnoringType14Walls())
            return true;
    }
    if (!ruleApplies && query.IsDetectingOrdinary() == 0)
        return true;
    return false;
}

int dBgW::DetectClsn(dBgCh_Gnd &ray) {
    return 0;
}

int dBgW::DetectClsn(dBgCh_Lin &ray) {
    return 0;
}

int dBgW::DetectClsn(dBgCh_SphCrr &sphere) {
    return 0;
}

void dBgW::BeforeClsn(dBgPi &res, dActor_c *actor, Vector3 &pos, Vector3_16 *motionAng,
                      Vector3_16 *ang) {
    beforeClsnCallback(*this, actor, res, pos, motionAng, ang);
}

int dBgW::TransformPos(const Vector3 &pos, Vector3 &res) {
    return 0;
}

s16 dBgW::GetAngularVelY() {
    return 0;
}

void dBgW::GetVelocity(Vector3 &res) {
    res.z = 0;
    res.y = res.z;
    res.x = res.y;
}

int dBgW::GetSlotIndex() const {
    return slotIdx;
}

void dBgW::SetSlotIndex(int slot) {
    slotIdx = slot;
}

void dBgW::ClearSlotIndex() {
    slotIdx = 0x18;
}

int dBgW::IsEnabled() {
    return slotIdx != 0x18;
}


/* Sets beforeClsnCallback (+0x18). */
extern "C" void func_020393d4(dBgW *collider, BeforeClsnCallback callback) {
    collider->beforeClsnCallback = callback;
}


/* Reads beforeClsnCallback (+0x18). */
BeforeClsnCallback dBgW::GetBeforeClsnCallback() const {
    return beforeClsnCallback;
}


/* Sets stoodOnCallback (+0x1c). */
extern "C" void func_020393c4(dBgW *collider, StoodOnCallback callback) {
    collider->stoodOnCallback = callback;
}


/* Reads stoodOnCallback (+0x1c). */
StoodOnCallback dBgW::GetStoodOnCallback() const {
    return stoodOnCallback;
}

dActor_c *dBgW::GetActor() const {
    return owner;
}

s32 dBgW::GetActorUniqueID() const {
    return ownerUniqueID;
}


/* Sets clipRadius (+0x0c). */
void dBgW::SetClipRadius(Fix12i radius) {
    clipRadius = radius;
}


/* Reads clipRadius (+0x0c). */
Fix12i dBgW::GetClipRadius() const {
    return clipRadius;
}


/* Sets clipOffsetY (+0x10). */
void dBgW::SetClipOffsetY(Fix12i offsetY) {
    clipOffsetY = offsetY;
}


/* Reads clipOffsetY (+0x10). */
Fix12i dBgW::GetClipOffsetY() const {
    return clipOffsetY;
}

void dBgW::UpdatePosWithTransform(dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
                                  Vector3_16 *motionAng, Vector3_16 *ang) {
    Vector3 prevPos;
    prevPos.x = pos.x;
    prevPos.y = pos.y;
    prevPos.z = pos.z;
    /* The ROM still tests the result; nothing follows either way. */
    if (!clsn.TransformPos(prevPos, pos))
        return;
}

void dBgW::UpdateAngsWithAngularVelY(dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
                                     Vector3_16 *motionAng, Vector3_16 *ang) {
    int angularVelocityY = clsn.GetAngularVelY();
    if (ang)
        ang->y += angularVelocityY;
    if (motionAng)
        motionAng->y += angularVelocityY;
}

void dBgW::UpdatePosAndAngs(dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
                            Vector3_16 *motionAng, Vector3_16 *ang) {
    UpdatePosWithTransform(clsn, clsnActor, res, pos, motionAng, ang);
    UpdateAngsWithAngularVelY(clsn, clsnActor, res, pos, motionAng, ang);
}

void dBgW::UpdatePosWithVelocity(dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
                                 Vector3_16 *motionAng, Vector3_16 *ang) {
    Vector3 velocity;
    clsn.GetVelocity(velocity);
    pos.x += velocity.x;
    pos.y += velocity.y;
    pos.z += velocity.z;
}

extern "C" void func_02039218(void) {
    int slot;
    for (slot = 0; slot < 0x18; slot++)
        data_020a0c80[slot] = 0;
}

void dScStage_c::ResetMeshColliders() {
    int slot;
    for (slot = 0; slot < 0x18; slot++)
        data_020a0c80[slot] = 0;
}

extern "C" void func_020391f0(void) {}

int dBgW::Enable(dActor_c *actor) {
    for (s32 slot = 0; slot < 0x18; slot++) {
        if (data_020a0c80[slot] == 0) {
            SetActor(actor);
            SetSlotIndex(slot);
            data_020a0c80[slot] = this;
            return 1;
        }
    }
    return 0;
}

int dBgW::Disable() {
    int slot = GetSlotIndex();
    if (slot != 0x18) {
        ClearSlotIndex();
        data_020a0c80[slot] = 0;
    }
    return 1;
}
