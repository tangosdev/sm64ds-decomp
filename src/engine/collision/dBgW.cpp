//cpp
/* dBgW -- the root of the mesh-collision collider family (KCL-backed walls).
 * ROM's own RTTI names it: _ZTS4dBgW @ 0x02099370, _ZTI4dBgW @ 0x02099378
 * (root __class_type_info), _ZTV4dBgW @ 0x02099388, 13 slots.
 *
 * This TU also carries the two collision-family members the original file
 * shared it with: dScStage_c::ResetMeshColliders, which clears the same
 * 0x18-entry registry data_020a0c80 that Enable fills, and the static
 * dBgCh::ShouldPassThroughImpl the DetectClsn queries consult.
 *
 * Deferred codegen emits definitions in reverse source order, so the file
 * opens with the two trailing setters and the ctor/dtor pair. The ten
 * func_0203938c..020393d4 accessors and func_020396c0/func_020396d0 keep
 * their func_ names: weak-emitted field accessors consumed by the dBgCh
 * query TUs and the derived colliders (callers in config/arm9/relocs.txt),
 * not dBgW members themselves.
 */
#include "dBgW.h"
#include "dBgCh.h"
#include "dScStage_c.h"
#include "dActor_c.h"
#include "decl_common.h"

extern "C" {
extern void *data_020a0c80[];
extern int SurfaceInfo_TestFlag0x20(int *p);
extern int func_02037e38(unsigned int *p);
}

// @symbol func_020396d0
extern "C" void func_020396d0(int *p, int v)
{
    p[17] = v;
}

// @symbol func_020396c0
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
    w->unk_1c = 0;
    w->unk_0c = -0x1000;
    w->unk_10 = 0;
}

// @symbol _ZN4dBgW9Virtual08Ev
void dBgW::Virtual08()
{
}

// @symbol func_020395fc
extern "C" void func_020395fc(dBgW *c, dActor_c *p)
{
    if (p == 0) {
        c->owner = 0;
        c->ownerUniqueID = -1;
    } else {
        c->owner = p;
        c->ownerUniqueID = p->uniqueID;
    }
}

// @symbol _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b
bool dBgCh::ShouldPassThroughImpl(void *p, const CLPS &clps_, const dBgCh &bg_, bool flag)
{
    int *clps = (int *)&clps_;
    unsigned char *bg = (unsigned char *)&bg_;
    int r4 = 0;

    if (SurfaceInfo_TestFlag0x20(clps)) {
        r4 = 1;
        if (func_0203547c(bg) == 0) return r4;
    }
    if (func_02037e20(clps)) {
        r4 = 1;
        if (func_0203543c(bg) == 0) return r4;
    }
    if (func_02037e2c(clps)) {
        r4 = 1;
        if (func_0203545c(bg) != 0) return r4;
    }
    if (func_02037e14(clps)) {
        r4 = 1;
        if (func_0203545c(bg) == 0) return r4;
    }
    if (func_02037e38((unsigned int *)clps) == 0x11) {
        r4 = 1;
        if (func_02035408(bg) != 0) return r4;
    }
    if (flag != 0 && func_02037e38((unsigned int *)clps) == 0x14) {
        r4 = 1;
        if (func_020353d4(bg) != 0) return r4;
    }
    if (r4 == 0) {
        if (func_020354b0(bg) == 0) return 1;
    }
    return 0;
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
    u8 v = slotIdx;
    if (v != 0x18)
        return 1;
    return 0;
}

// @symbol func_020393d4
extern "C" void func_020393d4(int *p, int v)
{
    p[6] = v;
}

// @symbol func_020393cc
extern "C" int func_020393cc(int *p)
{
    return p[6];
}

// @symbol func_020393c4
extern "C" void func_020393c4(int *p, int v)
{
    p[7] = v;
}

// @symbol func_020393bc
extern "C" int func_020393bc(int *p)
{
    return p[7];
}

// @symbol func_020393b4
extern "C" int func_020393b4(int *p)
{
    return p[1];
}

// @symbol func_020393ac
extern "C" int func_020393ac(int *p)
{
    return p[2];
}

// @symbol func_020393a4
extern "C" void func_020393a4(int *p, int v)
{
    p[3] = v;
}

// @symbol func_0203939c
extern "C" int func_0203939c(int *p)
{
    return p[3];
}

// @symbol func_02039394
extern "C" void func_02039394(int *p, int v)
{
    p[4] = v;
}

// @symbol func_0203938c
extern "C" int func_0203938c(int *p)
{
    return p[4];
}

// @symbol _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdatePosWithTransform(dBgW &clsn, dActor_c *clsnActor,
                                              dBgPi &res, Vector3 &pos,
                                              Vector3_16 *motionAng, Vector3_16 *ang)
{
    Vector3 local;
    local.x = pos.x;
    local.y = pos.y;
    local.z = pos.z;
    if (clsn.TransformPos(local, pos) == 0)
        return;
    return;
}

// @symbol _ZN4dBgW25UpdateAngsWithAngularVelYERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
void dBgW::UpdateAngsWithAngularVelY(dBgW &clsn, dActor_c *clsnActor,
                                                 dBgPi &res, Vector3 &pos,
                                                 Vector3_16 *motionAng, Vector3_16 *ang)
{
    int angY = clsn.GetAngularVelY();
    if (ang) {
        short *py = (short *)(&ang->y);
        *py = *py + angY;
    }
    if (motionAng) {
        short *py = (short *)(&motionAng->y);
        *py = *py + angY;
    }
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
    pos.x = pos.x + vel.x;
    {
        /* launder: the ROM re-reads through the materialized addresses */
        int *py = (int *)(&pos.y);
        *py = *py + vel.y;
    }
    {
        int *pz = (int *)(&pos.z);
        *pz = *pz + vel.z;
    }
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
    s32 i = 0;
    for (;;) {
        if (data_020a0c80[i] == 0) {
            func_020395fc(this, actor);
            func_020393fc(this, i);
            data_020a0c80[i] = this;
            return 1;
        }
        i++;
        if (i >= 0x18)
            break;
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
