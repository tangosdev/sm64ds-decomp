//cpp
/* leftovers:
 * The func_0203938c..020393d4 accessors and func_020396c0 / func_020396d0
 * retain their existing free-function interfaces for the query TUs and
 * derived colliders. Their original member/free-function form is not
 * established by the address names. Definitions retain their source order.
 * ShouldPassThroughImpl keeps the int* view of CLPS and the unsigned char*
 * view of the query for the flag tests that are still free functions.
 * data_020a0c80 (declared in dBgW.h) is the 0x18-entry enabled-collider
 * table Enable fills and dScStage_c::ResetMeshColliders clears.
 */
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

/* Preserve the address-based helper ABI while using the declared callback
   type; BeforeClsnCallback is dBgW.h's. */
typedef void (*StoodOnCallback)(dBgW *, dActor_c *, dActor_c *);

// @symbol func_020396d0

/* Stores dBgW_Kc::depthDirMinDot (+0x44), the sphere query's face-dot threshold. */
extern "C" void func_020396d0(int *collider, int faceDotThreshold) {
    reinterpret_cast<dBgW_Kc *>(collider)->depthDirMinDot = faceDotThreshold;
}

// @symbol func_020396c0

/* Stores dBgW_Kc::edgeToleranceShift (+0x48), clamped at 0. */
extern "C" void func_020396c0(void *collider, int edgeToleranceShift) {
    dBgW_Kc *mesh = static_cast<dBgW_Kc *>(collider);
    if (edgeToleranceShift < 0)
        edgeToleranceShift = 0;
    mesh->edgeToleranceShift = edgeToleranceShift;
}

// @symbol _ZN4dBgWC2Ev
dBgW::dBgW() {
    func_02039624((char *)this);
}

// @symbol _ZN4dBgWD2Ev
// @symbol _ZN4dBgWD0Ev
// @symbol _ZN4dBgWD1Ev
dBgW::~dBgW() {}

// @symbol func_02039624
extern "C" void func_02039624(char *object) {
    dBgW *collider = (dBgW *)object;
    collider->owner = 0;
    collider->ownerUniqueID = -1;
    collider->slotIdx = 0x18;
    collider->beforeClsnCallback = 0;
    collider->stoodOnCallback = 0;
    collider->clipRadius = -0x1000;
    collider->clipOffsetY = 0;
}

// @symbol _ZN4dBgW9Virtual08Ev
void dBgW::Virtual08() {}

// @symbol func_020395fc

/* Binds the collider to its owning actor, or clears the binding. */
extern "C" void func_020395fc(dBgW *collider, dActor_c *actor) {
    if (actor == 0) {
        collider->owner = 0;
        collider->ownerUniqueID = -1;
    } else {
        collider->owner = actor;
        collider->ownerUniqueID = actor->uniqueID;
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
       ordinary ground (bit 1). The CLPS predicates and the remaining flag
       tests are C helpers taking raw word and byte views. */
    int *clpsWords = (int *)&surfaceProperties;
    unsigned char *queryBytes = (unsigned char *)&query;
    bool ruleApplies = false;

    if (SurfaceInfo_TestFlag0x20(clpsWords)) {
        ruleApplies = true;
        if (func_0203547c(queryBytes) == 0)
            return true;
    }
    if (func_02037e20(clpsWords)) {
        ruleApplies = true;
        if (func_0203543c(queryBytes) == 0)
            return true;
    }
    if (func_02037e2c(clpsWords)) {
        ruleApplies = true;
        if (func_0203545c(queryBytes) != 0)
            return true;
    }
    if (func_02037e14(clpsWords)) {
        ruleApplies = true;
        if (func_0203545c(queryBytes) == 0)
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
    if (!ruleApplies && func_020354b0(queryBytes) == 0)
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
    beforeClsnCallback(this, actor, &res, &pos, motionAng, ang);
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

// @symbol func_02039404
extern "C" int func_02039404(dBgW *collider) {
    return collider->slotIdx;
}

// @symbol func_020393fc
extern "C" void func_020393fc(dBgW *collider, int slot) {
    collider->slotIdx = slot;
}

// @symbol func_020393f0
extern "C" void func_020393f0(dBgW *collider) {
    collider->slotIdx = 0x18;
}

// @symbol _ZN4dBgW9IsEnabledEv
int dBgW::IsEnabled() {
    return slotIdx != 0x18;
}

// @symbol func_020393d4

/* Sets beforeClsnCallback (+0x18). */
extern "C" void func_020393d4(int *collider, int callbackAddress) {
    reinterpret_cast<dBgW *>(collider)->beforeClsnCallback =
        reinterpret_cast<BeforeClsnCallback>(callbackAddress);
}

// @symbol func_020393cc

/* Reads beforeClsnCallback (+0x18). */
extern "C" int func_020393cc(int *collider) {
    return reinterpret_cast<int>(reinterpret_cast<dBgW *>(collider)->beforeClsnCallback);
}

// @symbol func_020393c4

/* Sets stoodOnCallback (+0x1c). */
extern "C" void func_020393c4(int *collider, int callbackAddress) {
    reinterpret_cast<dBgW *>(collider)->stoodOnCallback =
        reinterpret_cast<StoodOnCallback>(callbackAddress);
}

// @symbol func_020393bc

/* Reads stoodOnCallback (+0x1c). */
extern "C" int func_020393bc(int *collider) {
    return reinterpret_cast<int>(reinterpret_cast<dBgW *>(collider)->stoodOnCallback);
}

// @symbol func_020393b4
extern "C" int func_020393b4(int *collider) {
    return reinterpret_cast<int>(reinterpret_cast<dBgW *>(collider)->owner);
}

// @symbol func_020393ac
extern "C" int func_020393ac(int *collider) {
    return reinterpret_cast<dBgW *>(collider)->ownerUniqueID;
}

// @symbol func_020393a4

/* Sets clipRadius (+0x0c). */
extern "C" void func_020393a4(int *collider, int clipDistance) {
    reinterpret_cast<dBgW *>(collider)->clipRadius = clipDistance;
}

// @symbol func_0203939c

/* Reads clipRadius (+0x0c). */
extern "C" int func_0203939c(int *collider) {
    return reinterpret_cast<dBgW *>(collider)->clipRadius;
}

// @symbol func_02039394

/* Sets clipOffsetY (+0x10). */
extern "C" void func_02039394(int *collider, int clipOffsetY) {
    reinterpret_cast<dBgW *>(collider)->clipOffsetY = clipOffsetY;
}

// @symbol func_0203938c

/* Reads clipOffsetY (+0x10). */
extern "C" int func_0203938c(int *collider) {
    return reinterpret_cast<dBgW *>(collider)->clipOffsetY;
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
            func_020395fc(this, actor);
            func_020393fc(this, slot);
            data_020a0c80[slot] = this;
            return 1;
        }
    }
    return 0;
}

// @symbol _ZN4dBgW7DisableEv
int dBgW::Disable() {
    int slot = func_02039404(this);
    if (slot != 0x18) {
        func_020393f0(this);
        data_020a0c80[slot] = 0;
    }
    return 1;
}
