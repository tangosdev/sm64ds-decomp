//cpp
/* Production translation unit for ov002/daObjPushblock_c's nine-method run,
 * 0x020b8bf0..0x020b910c. The adjacent PushBlock_Spawn function starts a
 * separate high-confidence TU and remains in PushBlock_Spawn.cpp.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * InitResources is the key function, so the inline destructor in the class
 * header makes mwccarm emit retail's D1-then-D0 pair with no D2 and no forcing
 * helper. The compiler-owned RTTI and vtable are audited through the manifest.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020b8bf0  src/_ZN16daObjPushblock_cD1Ev.cpp
 *   [1] 0x020b8c3c  src/_ZN16daObjPushblock_cD0Ev.cpp
 *   [2] 0x020b8c9c  src/_ZN16daObjPushblock_c4KillEv.cpp
 *   [3] 0x020b8d14  src/_ZN16daObjPushblock_c15OnHitByMegaCharER6Player.cpp
 *   [4] 0x020b8d3c  src/_ZN16daObjPushblock_c8OnPushedER8dActor_c.cpp
 *   [5] 0x020b8d68  src/_ZN16daObjPushblock_c16CleanupResourcesEv.cpp
 *   [6] 0x020b8dac  src/_ZN16daObjPushblock_c6RenderEv.cpp
 *   [7] 0x020b8dd4  src/_ZN16daObjPushblock_c8BehaviorEv.c
 *   [8] 0x020b8fe0  src/_ZN16daObjPushblock_c13InitResourcesEv.cpp
 */

#include "daObjPushblock_c.h"
#include "Sound.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "decl_common.h"
#include "decl_Platform.h"
#include "dBgCh_Gnd.h"

/* These seams preserve retail ABI details not expressible through the current
 * typed headers. The Fix12<int>-by-value entries change caller codegen when
 * respelled as C++ calls, and UpdateContinuous must retain the retail veneer.
 * The remaining declarations are untyped engine free functions and globals. */
extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
extern Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *thiz, int a, int b);
extern int Vec3_Dist(Vector3 *a, Vector3 *b);
extern int Vec3_HorzDist(Vector3 *a, Vector3 *b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *k, void *m, int fix, short s, void *clps);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *a, int b, int c, void *d, int e);
extern SharedFilePtr data_ov002_0210df9c;
extern SharedFilePtr data_ov002_0210df94;
}

namespace Sound {
u32 PlayLong(u32 handle, u32 bank, u32 soundId, const Vector3 &pos, s16 pitch);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN16daObjPushblock_c13InitResourcesEv, 0x020b8fe0, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjPushblock_c13InitResourcesEv
/* recovered: vtable identified, shared common types, declarations from a shared header
 *
 * daObjPushblock_c::InitResources -- vtable slot 0, ov002 0x020b8fe0. Attributed by
 * the vtable: _ZTV16daObjPushblock_c (ov002 0x021096b0) carries this address at slot
 * 0, where _ZTV10dBgActor_c carries fBase_c's generic 0x02043c80.
 *
 * A real method over the shared header. It used to be an extern "C" free
 * function over a raw `char *self` reading every field by hand offset; the
 * conversion is byte-exact under the pinned 2004/b56. */
int daObjPushblock_c::InitResources()
{
    Vector3 v;
    BMD_File *bmd;
    KCL_File *kcl;

    bmd = (BMD_File *)Model::LoadFile(data_ov002_0210df9c);
    mModel.SetFile(bmd, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov002_0210df94);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov002_0210d7b4);

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x96000, 0x96000, 0, 0);

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y + 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(v, 0);
    mGroundY = v.y;
    if (ground.DetectClsn())
        mGroundY = ground.clsnY;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN16daObjPushblock_c8BehaviorEv, 0x020b8dd4, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjPushblock_c8BehaviorEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header
 *
 * daObjPushblock_c::Behavior -- vtable slot 6, ov002 0x020b8dd4. Attributed by the
 * vtable: _ZTV16daObjPushblock_c carries this address at slot 6, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043b24.
 *
 * The real member form is byte-exact in this whole-TU context. Keeping the
 * home-coordinate declarations beside their first uses reproduces the retail
 * load/register order without raw field offsets. */
int daObjPushblock_c::Behavior()
{
    Vector3 v;
    Vector3 dst;
    Vector3 pos;
    dActor_c *q;

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(this, 0x1800, 0, 0, 0x96000) != 0) {
        return 1;
    }

    if (mHorzSpeed != 0) {
        v.x = 0;
        dst.x = 0;
        dst.y = 0;
        dst.z = 0;
        v.y = 0x96000;
        v.z = 0x96000;
        Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
        MulVec3Mat4x3(&v, &data_020a0e68, &dst);
        AddVec3(&dst, (Vector3 *)&mPosX, &dst);
        dBgCh_Gnd ray;
        ray.SetObjAndPos(dst, 0);
        if (ray.DetectClsn()) {
            if (mGroundY != ray.clsnY) {
                mHorzSpeed = 0;
            }
        } else {
            mHorzSpeed = 0;
        }
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
    }

    func_020393a4((int *)&mMeshCollider, 0x12c000);
    func_02039394((int *)&mMeshCollider, 0x96000);
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x3e8000, 0x96000);

    if (mHorzSpeed != 0 && Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&mPrevPosX) != 0) {
        mSlideSound = Sound::PlayLong(
            mSlideSound, 3, 0x97, *(Vector3 *)&mCamSpacePosX, 0);
    }

    if (Vec3_HorzDist((Vector3 *)&mHomePosX, (Vector3 *)&mPosX) >= 0x12c000) {
        if (mLinkedActor != 0) {
            if (mLinkedActor->actorID == 0x149) {
                const int homeX = mHomePosX;
                pos.x = homeX;
                const int homeY = mHomePosY;
                pos.y = homeY;
                const int homeZ = mHomePosZ;
                pos.z = homeZ;
                pos.y = homeY + 0x96000;
                q = mLinkedActor;
                q->mPosX = homeX;
                q->mPosY = pos.y;
                q->mPosZ = pos.z;
                func_ov002_020f0438(mLinkedActor);
            }
            mLinkedActor = 0;
        }
    }

    mHorzSpeed = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN16daObjPushblock_c6RenderEv, 0x020b8dac, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjPushblock_c6RenderEv
/* daObjPushblock_c::Render -- vtable slot 9, ov002 0x020b8dac. Attributed by the
 * vtable: _ZTV16daObjPushblock_c carries this address at slot 9, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043af0.
 *
 * Draws the block's own Model sub-object at +0xd4 (dBgActor_c::mModel, inherited)
 * through Model::Render (vtable slot 5). mwccarm dispatches the embedded-member
 * call through the vtable rather than devirtualizing it, so the ROM's indirect
 * call is reproduced from the plain named form. */
int daObjPushblock_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN16daObjPushblock_c16CleanupResourcesEv, 0x020b8d68, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjPushblock_c16CleanupResourcesEv
/* recovered: real C++ method */
/* daObjPushblock_c::CleanupResources() -- takes the block's dBgW collision out of the
 * world if it is still registered, then gives back the two shared files. */
int daObjPushblock_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov002_0210df9c.Release();
    data_ov002_0210df94.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjPushblock_c8OnPushedER8dActor_c, 0x020b8d3c, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjPushblock_c8OnPushedER8dActor_c
/* recovered: named members + real C++ method */
/* daObjPushblock_c::OnPushed(dActor_c &) -- slot 25. Takes the pusher's facing as its
 * own slide direction and picks the speed from the pusher's state: 2 (a dash?)
 * gets the fast 0x8000, anything else 0x4000. */
/* THE MISSING RETURN IS DELIBERATE AND LOAD-BEARING. The header declares slot 25
 * as `int`, but this body sets no return value and the ROM's 0x2c bytes leave r0
 * holding whatever the caller had. Adding `return 0;` to satisfy the declaration
 * emits a real `mov r0,#0` and BREAKS THE MATCH -- measured 2026-08-22.
 *
 * Slot 25's return type is UNCONSTRAINED by evidence: this is the only recovered
 * OnPushed body in the tree, and a fall-off-the-end body compiles identically
 * under `int` and `void`. The `int` comes from the family convention and from
 * the neighbouring slots (OnAttacked1/2, OnHitFromUnderneath,
 * OnHitByCannonBlastedChar) which DO return values -- the same unfalsifiable
 * shape already analysed for slot 19, where four constrained overrides settle it.
 *
 * Harmless for the ROM build. NOT harmless for the host port: a C++ caller that
 * reads this result gets garbage. If the port ever branches on OnPushed's return,
 * that is the bug, and the fix is upstream in dActor_c.h's slot-25 return type --
 * not a `return 0;` here, which would desync the bytes. */
int daObjPushblock_c::OnPushed(dActor_c &other)
{
    dActor_c *pusher = &other;
    if (pusher == 0) return;
    mPrevAngleY = pusher->mAngleY;
    if (pusher->param1 == 2) mHorzSpeed = 0x8000;
    else mHorzSpeed = 0x4000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjPushblock_c15OnHitByMegaCharER6Player, 0x020b8d14, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjPushblock_c15OnHitByMegaCharER6Player
/* daObjPushblock_c::OnHitByMegaChar -- vtable slot 27, ov002 0x020b8d14. Attributed by
 * the vtable: _ZTV16daObjPushblock_c carries 0x020b8d14 at slot 27 -- vtable + 0x6c --
 * where _ZTV10dBgActor_c carries dActor_c's generic 0x02010124. Confirmed with
 * tools/mangle.py: _ZN16daObjPushblock_c15OnHitByMegaCharER6Player.
 *
 * Same idiom as SlidingIce::OnHitByMegaChar: Player::IncMegaKillCount is a real
 * method, and the trailing unqualified Kill() dispatches virtually through slot
 * 31. daObjPushblock_c does override slot 31 (its own _ZN16daObjPushblock_c4KillEv), so this
 * reaches daObjPushblock_c::Kill through the vtable rather than the base. */
void daObjPushblock_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjPushblock_c4KillEv, 0x020b8c9c, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjPushblock_c4KillEv
/* daObjPushblock_c::Kill() -- vtable slot 31, ov002 0x020b8c9c. Attributed by the
 * vtable: _ZTV16daObjPushblock_c carries this address at slot 31 -- vtable + 0x7c --
 * where _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv, and slot 30 carries
 * dActor_c's 0x020100dc in both tables. So this is daObjPushblock_c's own override of
 * the one virtual dBgActor_c adds.
 *
 * Same shape as dBgActor_c::Kill (its base's slot 31): spawn a poof particle
 * above the block, poof-dust at that point, play the break sound at
 * mCamSpacePos, then mark for destruction. Particle::System::NewSimple stays
 * spelled as its mangled name because its coordinates are Fix12<int> BY VALUE;
 * declaring the true types changes how the caller passes them and breaks the
 * bytes (notes/mwccarm-codegen.md 6az, and
 * src/_ZN8dActor_c10PoofDustAtERK7Vector3.cpp). */
void daObjPushblock_c::Kill()
{
    Vector3 vec;
    Vector3 vec2;
    vec.x = mPosX;
    vec.y = mPosY;
    vec.z = mPosZ;
    vec.y += 0x96000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, vec.x, vec.y, vec.z);
    vec2.x = vec.x;
    vec2.y = vec.y;
    vec2.z = vec.z;
    PoofDustAt(vec2);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
