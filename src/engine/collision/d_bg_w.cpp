//cpp
/* leftovers:
 * The func_0203938c..020393d4 accessors and func_020396c0 / func_020396d0
 * stay free functions. The query TUs and the derived colliders call them
 * by address; they are not dBgW methods. The int * ones keep that
 * parameter type because the actor TUs that call them declare it so.
 * Definitions stay in reverse source order, so the setters and the
 * constructor come first.
 * ShouldPassThroughImpl keeps the int* view of CLPS and the unsigned char*
 * view of the query. data_020a0c80 is the 0x18-entry enabled-collider
 * table Enable fills and dScStage_c::ResetMeshColliders clears.
 */
/* dBgW: root of the KCL collider family. Type name 0x02099370, typeinfo
 * 0x02099378, vtable 0x02099388, 13 slots. This TU also defines
 * dScStage_c::ResetMeshColliders and dBgCh::ShouldPassThroughImpl.
 */
#include "dBgW.h"
#include "dBgCh.h"
#include "dScStage_c.h"
#include "dActor_c.h"
#include "decl_common.h"

extern "C" {
/* The enabled-collider table: Enable puts a collider in the first free
   entry and records the index in slotIdx. The query TUs walk it. */
extern dBgW *data_020a0c80[];
extern int SurfaceInfo_TestFlag0x20(int *p);
extern int func_02037e38(unsigned int *p);
}

// @symbol func_020396d0
/* Stores dBgW_Kc::unk_44 (+0x44), the sphere query's face-dot threshold. */
extern "C" void func_020396d0(int *p, int v)
{
    p[17] = v;
}

// @symbol func_020396c0
/* Stores dBgW_Kc::unk_48 (+0x48), the edge-tolerance shift, clamped at 0. */
extern "C" void func_020396c0(void *p, int v)
{
    int *pi = (int *)p;
    if (v < 0) v = 0;
    pi[0x12] = v;
}

// @symbol _ZN4dBgWC2Ev
dBgW::dBgW()
{
    func_02039624((char *)this);
}

// @symbol _ZN4dBgWD2Ev
// @symbol _ZN4dBgWD0Ev
// @symbol _ZN4dBgWD1Ev
dBgW::~dBgW()
{
}

// @symbol func_02039624
extern "C" void func_02039624(char *o)
{
    dBgW *w = (dBgW *)o;
    w->owner = 0;
    w->ownerUniqueID = -1;
    w->slotIdx = 0x18;
    w->beforeClsnCallback = 0;
    w->stoodOnCallback = 0;
    w->clipRadius = -0x1000;
    w->clipOffsetY = 0;
}

// @symbol _ZN4dBgW9Virtual08Ev
void dBgW::Virtual08()
{
}

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
bool dBgCh::ShouldPassThroughImpl(void *p, const CLPS &clps_, const dBgCh &bg_, bool flag)
{
    /* Each special surface asks whether the query detects its kind, through
       dBgCh::mDetectFlags: a water surface (CLPS 0x20) needs the water bit
       (2), CLPS bit 0x02000000 needs the toxic bit (8). A surface no rule
       covers stops only queries that detect ordinary ground (bit 1). The
       CLPS predicates and the flag tests are C helpers taking raw word and
       byte views. */
    int *clps = (int *)&clps_;
    unsigned char *bg = (unsigned char *)&bg_;
    bool ruleApplies = false;

    if (SurfaceInfo_TestFlag0x20(clps)) {
        ruleApplies = true;
        if (func_0203547c(bg) == 0)
            return true;
    }
    if (func_02037e20(clps)) {
        ruleApplies = true;
        if (func_0203543c(bg) == 0)
            return true;
    }
    if (func_02037e2c(clps)) {
        ruleApplies = true;
        if (func_0203545c(bg) != 0)
            return true;
    }
    if (func_02037e14(clps)) {
        ruleApplies = true;
        if (func_0203545c(bg) == 0)
            return true;
    }
    if (func_02037e38((unsigned int *)clps) == 0x11) {
        ruleApplies = true;
        if (func_02035408(bg) != 0)
            return true;
    }
    if (flag && func_02037e38((unsigned int *)clps) == 0x14) {
        ruleApplies = true;
        if (func_020353d4(bg) != 0)
            return true;
    }
    if (!ruleApplies && func_020354b0(bg) == 0)
        return true;
    return false;
}

// @symbol _ZN4dBgW10DetectClsnER9dBgCh_Gnd
int dBgW::DetectClsn(dBgCh_Gnd &ray)
{
    return 0;
}

// @symbol _ZN4dBgW10DetectClsnER9dBgCh_Lin
int dBgW::DetectClsn(dBgCh_Lin &ray)
{
    return 0;
}

// @symbol _ZN4dBgW10DetectClsnER12dBgCh_SphCrr
int dBgW::DetectClsn(dBgCh_SphCrr &sphere)
{
    return 0;
}

// @symbol _ZN4dBgW10BeforeClsnER5dBgPiP8dActor_cR7Vector3P10Vector3_16S7_
void dBgW::BeforeClsn(dBgPi &res, dActor_c *actor, Vector3 &pos,
                                  Vector3_16 *motionAng, Vector3_16 *ang)
{
    beforeClsnCallback(this, actor, &res, &pos, motionAng, ang);
}

// @symbol _ZN4dBgW12TransformPosERK7Vector3RS0_
int dBgW::TransformPos(const Vector3 &pos, Vector3 &res)
{
    return 0;
}

// @symbol _ZN4dBgW14GetAngularVelYEv
s16 dBgW::GetAngularVelY()
{
    return 0;
}

// @symbol _ZN4dBgW11GetVelocityER7Vector3
void dBgW::GetVelocity(Vector3 &res)
{
    res.z = 0;
    res.y = res.z;
    res.x = res.y;
}

// @symbol func_02039404
extern "C" int func_02039404(dBgW *c)
{
    return c->slotIdx;
}

// @symbol func_020393fc
extern "C" void func_020393fc(dBgW *c, int v)
{
    c->slotIdx = v;
}

// @symbol func_020393f0
extern "C" void func_020393f0(dBgW *c)
{
    c->slotIdx = 0x18;
}

// @symbol _ZN4dBgW9IsEnabledEv
int dBgW::IsEnabled()
{
    return slotIdx != 0x18;
}

// @symbol func_020393d4
/* Sets beforeClsnCallback (+0x18). */
extern "C" void func_020393d4(int *p, int v)
{
    p[6] = v;
}

// @symbol func_020393cc
/* Reads beforeClsnCallback (+0x18). */
extern "C" int func_020393cc(int *p)
{
    return p[6];
}

// @symbol func_020393c4
/* Sets stoodOnCallback (+0x1c). */
extern "C" void func_020393c4(int *p, int v)
{
    p[7] = v;
}

// @symbol func_020393bc
/* Reads stoodOnCallback (+0x1c). */
extern "C" int func_020393bc(int *p)
{
    return p[7];
}

// @symbol func_020393b4
extern "C" int func_020393b4(int *p)
{
    return (int)((dBgW *)p)->owner;
}

// @symbol func_020393ac
extern "C" int func_020393ac(int *p)
{
    return ((dBgW *)p)->ownerUniqueID;
}

// @symbol func_020393a4
/* Sets clipRadius (+0x0c). */
extern "C" void func_020393a4(int *p, int v)
{
    p[3] = v;
}

// @symbol func_0203939c
/* Reads clipRadius (+0x0c). */
extern "C" int func_0203939c(int *p)
{
    return p[3];
}

// @symbol func_02039394
/* Sets clipOffsetY (+0x10). */
extern "C" void func_02039394(int *p, int v)
{
    p[4] = v;
}

// @symbol func_0203938c
/* Reads clipOffsetY (+0x10). */
extern "C" int func_0203938c(int *p)
{
    return p[4];
}

// @symbol _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdatePosWithTransform(dBgW &clsn, dActor_c *clsnActor,
                                              dBgPi &res, Vector3 &pos,
                                              Vector3_16 *motionAng, Vector3_16 *ang)
{
    Vector3 prevPos;
    prevPos.x = pos.x;
    prevPos.y = pos.y;
    prevPos.z = pos.z;
    /* The ROM still tests the result; nothing follows either way. */
    if (!clsn.TransformPos(prevPos, pos))
        return;
}

// @symbol _ZN4dBgW25UpdateAngsWithAngularVelYERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdateAngsWithAngularVelY(dBgW &clsn, dActor_c *clsnActor,
                                                 dBgPi &res, Vector3 &pos,
                                                 Vector3_16 *motionAng, Vector3_16 *ang)
{
    int angY = clsn.GetAngularVelY();
    if (ang)
        ang->y += angY;
    if (motionAng)
        motionAng->y += angY;
}

// @symbol _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdatePosAndAngs(dBgW &clsn, dActor_c *clsnActor,
                                        dBgPi &res, Vector3 &pos,
                                        Vector3_16 *motionAng, Vector3_16 *ang)
{
    UpdatePosWithTransform(clsn, clsnActor, res, pos, motionAng, ang);
    UpdateAngsWithAngularVelY(clsn, clsnActor, res, pos, motionAng, ang);
}

// @symbol _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdatePosWithVelocity(dBgW &clsn, dActor_c *clsnActor,
                                             dBgPi &res, Vector3 &pos,
                                             Vector3_16 *motionAng, Vector3_16 *ang)
{
    Vector3 vel;
    clsn.GetVelocity(vel);
    pos.x += vel.x;
    pos.y += vel.y;
    pos.z += vel.z;
}

// @symbol func_02039218
extern "C" void func_02039218(void)
{
    int i;
    for (i = 0; i < 0x18; i++) data_020a0c80[i] = 0;
}

// @symbol _ZN10dScStage_c18ResetMeshCollidersEv
void dScStage_c::ResetMeshColliders()
{
    int i;
    for (i = 0; i < 0x18; i++) data_020a0c80[i] = 0;
}

// @symbol func_020391f0
extern "C" void func_020391f0(void)
{
}

// @symbol _ZN4dBgW6EnableEP8dActor_c
int dBgW::Enable(dActor_c *actor)
{
    for (s32 i = 0; i < 0x18; i++) {
        if (data_020a0c80[i] == 0) {
            func_020395fc(this, actor);
            func_020393fc(this, i);
            data_020a0c80[i] = this;
            return 1;
        }
    }
    return 0;
}

// @symbol _ZN4dBgW7DisableEv
int dBgW::Disable()
{
    int i = func_02039404(this);
    if (i != 0x18) {
        func_020393f0(this);
        data_020a0c80[i] = 0;
    }
    return 1;
}
