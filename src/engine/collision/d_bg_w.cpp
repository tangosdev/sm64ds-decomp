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

// @symbol _ZN7dBgW_Kc17SetDepthDirMinDotEi

/* Stores dBgW_Kc::depthDirMinDot (+0x44), the sphere query's face-dot threshold. */
void dBgW_Kc::SetDepthDirMinDot(Fix12i value) {
    depthDirMinDot = value;
}

// @symbol _ZN7dBgW_Kc21SetEdgeToleranceShiftEi

/* Stores dBgW_Kc::edgeToleranceShift (+0x48), clamped at 0. */
void dBgW_Kc::SetEdgeToleranceShift(int value) {
    if (value < 0)
        value = 0;
    edgeToleranceShift = value;
}

// @symbol _ZN4dBgWC2Ev
dBgW::dBgW() {
    Reset();
}

// @symbol _ZN4dBgWD2Ev
// @symbol _ZN4dBgWD0Ev
// @symbol _ZN4dBgWD1Ev
dBgW::~dBgW() {}

// @symbol _ZN4dBgW5ResetEv
void dBgW::Reset() {
    owner = 0;
    ownerUniqueID = -1;
    slotIdx = 0x18;
    beforeClsnCallback = 0;
    stoodOnCallback = 0;
    clipRadius = -0x1000;
    clipOffsetY = 0;
}

// @symbol _ZN4dBgW9Virtual08Ev
void dBgW::Virtual08() {}

// @symbol _ZN4dBgW8SetActorEP8dActor_c

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

// @symbol _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b
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

// @symbol _ZN4dBgW10DetectClsnER9dBgCh_Gnd
int dBgW::DetectClsn(dBgCh_Gnd &ray) {
    return 0;
}

// @symbol _ZN4dBgW10DetectClsnER9dBgCh_Lin
int dBgW::DetectClsn(dBgCh_Lin &ray) {
    return 0;
}

// @symbol _ZN4dBgW10DetectClsnER12dBgCh_SphCrr
int dBgW::DetectClsn(dBgCh_SphCrr &sphere) {
    return 0;
}

// @symbol _ZN4dBgW10BeforeClsnER5dBgPiP8dActor_cR7Vector3P10Vector3_16S7_
void dBgW::BeforeClsn(dBgPi &res, dActor_c *actor, Vector3 &pos, Vector3_16 *motionAng,
                      Vector3_16 *ang) {
    beforeClsnCallback(*this, actor, res, pos, motionAng, ang);
}

// @symbol _ZN4dBgW12TransformPosERK7Vector3RS0_
int dBgW::TransformPos(const Vector3 &pos, Vector3 &res) {
    return 0;
}

// @symbol _ZN4dBgW14GetAngularVelYEv
s16 dBgW::GetAngularVelY() {
    return 0;
}

// @symbol _ZN4dBgW11GetVelocityER7Vector3
void dBgW::GetVelocity(Vector3 &res) {
    res.z = 0;
    res.y = res.z;
    res.x = res.y;
}

// @symbol _ZNK4dBgW12GetSlotIndexEv
int dBgW::GetSlotIndex() const {
    return slotIdx;
}

// @symbol _ZN4dBgW12SetSlotIndexEi
void dBgW::SetSlotIndex(int slot) {
    slotIdx = slot;
}

// @symbol _ZN4dBgW14ClearSlotIndexEv
void dBgW::ClearSlotIndex() {
    slotIdx = 0x18;
}

// @symbol _ZN4dBgW9IsEnabledEv
int dBgW::IsEnabled() {
    return slotIdx != 0x18;
}

// @symbol func_020393d4

/* Sets beforeClsnCallback (+0x18). */
extern "C" void func_020393d4(dBgW *collider, BeforeClsnCallback callback) {
    collider->beforeClsnCallback = callback;
}

// @symbol _ZNK4dBgW21GetBeforeClsnCallbackEv

/* Reads beforeClsnCallback (+0x18). */
BeforeClsnCallback dBgW::GetBeforeClsnCallback() const {
    return beforeClsnCallback;
}

// @symbol func_020393c4

/* Sets stoodOnCallback (+0x1c). */
extern "C" void func_020393c4(int *collider, int callbackAddress) {
    reinterpret_cast<dBgW *>(collider)->stoodOnCallback =
        reinterpret_cast<StoodOnCallback>(callbackAddress);
}

// @symbol _ZNK4dBgW18GetStoodOnCallbackEv

/* Reads stoodOnCallback (+0x1c). */
StoodOnCallback dBgW::GetStoodOnCallback() const {
    return stoodOnCallback;
}

// @symbol _ZNK4dBgW8GetActorEv
dActor_c *dBgW::GetActor() const {
    return owner;
}

// @symbol _ZNK4dBgW16GetActorUniqueIDEv
s32 dBgW::GetActorUniqueID() const {
    return ownerUniqueID;
}

// @symbol _ZN4dBgW13SetClipRadiusEi

/* Sets clipRadius (+0x0c). */
void dBgW::SetClipRadius(Fix12i radius) {
    clipRadius = radius;
}

// @symbol _ZNK4dBgW13GetClipRadiusEv

/* Reads clipRadius (+0x0c). */
Fix12i dBgW::GetClipRadius() const {
    return clipRadius;
}

// @symbol _ZN4dBgW14SetClipOffsetYEi

/* Sets clipOffsetY (+0x10). */
void dBgW::SetClipOffsetY(Fix12i offsetY) {
    clipOffsetY = offsetY;
}

// @symbol _ZNK4dBgW14GetClipOffsetYEv

/* Reads clipOffsetY (+0x10). */
Fix12i dBgW::GetClipOffsetY() const {
    return clipOffsetY;
}

// @symbol _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
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

// @symbol _ZN4dBgW25UpdateAngsWithAngularVelYERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdateAngsWithAngularVelY(dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
                                     Vector3_16 *motionAng, Vector3_16 *ang) {
    int angularVelocityY = clsn.GetAngularVelY();
    if (ang)
        ang->y += angularVelocityY;
    if (motionAng)
        motionAng->y += angularVelocityY;
}

// @symbol _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdatePosAndAngs(dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
                            Vector3_16 *motionAng, Vector3_16 *ang) {
    UpdatePosWithTransform(clsn, clsnActor, res, pos, motionAng, ang);
    UpdateAngsWithAngularVelY(clsn, clsnActor, res, pos, motionAng, ang);
}

// @symbol _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdatePosWithVelocity(dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
                                 Vector3_16 *motionAng, Vector3_16 *ang) {
    Vector3 velocity;
    clsn.GetVelocity(velocity);
    pos.x += velocity.x;
    pos.y += velocity.y;
    pos.z += velocity.z;
}

// @symbol func_02039218
extern "C" void func_02039218(void) {
    int slot;
    for (slot = 0; slot < 0x18; slot++)
        data_020a0c80[slot] = 0;
}

// @symbol _ZN10dScStage_c18ResetMeshCollidersEv
void dScStage_c::ResetMeshColliders() {
    int slot;
    for (slot = 0; slot < 0x18; slot++)
        data_020a0c80[slot] = 0;
}

// @symbol func_020391f0
extern "C" void func_020391f0(void) {}

// @symbol _ZN4dBgW6EnableEP8dActor_c
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

// @symbol _ZN4dBgW7DisableEv
int dBgW::Disable() {
    int slot = GetSlotIndex();
    if (slot != 0x18) {
        ClearSlotIndex();
        data_020a0c80[slot] = 0;
    }
    return 1;
}
