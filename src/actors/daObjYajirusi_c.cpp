//cpp
/**
 * Arrow sign (YAJIRUSI_L / YAJIRUSI_R).
 *
 * Mesh-collision signpost. actorID 0x12b (YAJIRUSI_L) selects file-table
 * column 0 and 0x12c (YAJIRUSI_R) selects column 1. Kill and OnHitByMegaChar
 * break it; OnAttacked1 does the same when the other actor is a Bob-omb
 * (actorID 0xce).
 *
 * daObjYajirusi_c_classInit_YAJIRUSI_R and _L stay those names. They are one
 * body (alloc 0x380, dBgActor_c C2, this vtable, dExtShadowModel_c C1 at +0x320)
 * and sit contiguous at 0x02137fd0..0x02138040, so the TU is
 * 0x02137be0..0x02138040 with no hole: 12 functions. Retail does not store
 * the factory spellings. Historical aliases ArrowSignRight_Spawn /
 * ArrowSignLeft_Spawn.
 *
 * SOURCE ORDER IS REVERSE ROM ORDER. mwccarm 2004/b56 emits one .text
 * section per function in reverse source order. Do not reorder. The
 * destructor is inline-first in daObjYajirusi_c.h, which is what puts D1
 * ahead of D0, and the inline base destructor emits a homeless
 * _ZN10dBgActor_cD2Ev (licensed deadstrip). The "// address (size)" line
 * above each definition is its ROM location.
 *
 * Leftover: dBgW_KcMbg::SetFile, dBgActor_c::UpdateKillByMegaChar,
 *   dBgActor_c::IsClsnInRangeOnScreen, dActor_c::DropShadowScaleXYZ and
 *   Particle::System::NewSimple stay mangled extern "C" calls. Each takes
 *   Fix12<int> by value, and a typed method call changes the caller's size.
 *   Fix12<int>{0} does not compile.
 * Leftover: func_02039394 / func_020393a4 store the collider words at +0x10
 *   and +0x0c (0xc0000 / 0xe0000, the profile ranges). Those names belong
 *   to dBgW.
 * Leftover: func_02012694 is Sound::Play of bank 3 at a pointer
 *   (src/engine/sound/Sound.cpp). OnHitByMegaChar passes &mCamSpacePosX. It is
 *   not Sound::PlayBank3 at 0x02012664.
 * Leftover: OnAttacked1 and OnHitByMegaChar take coined references. A
 *   reference and a pointer mangle differently and generate the same ARM
 *   for these bodies.
 * Leftover: Kill copies dustPos memberwise. Vector3's destructor turns a
 *   whole-object assignment into ldm/stm, four instructions where the ROM
 *   stores six.
 * Leftover: mShadowMat stays a u8 at +0x348. A Matrix4x3 member would run
 *   Vector3's destructor from this class's D1. func_ov098_02137c8c stores
 *   the translation at +0x24 of that marker, not through Matrix4x3::t.
 * Leftover: data_ov098_0213c380 / 0213c384 / 0213c388 are overlapping
 *   column views (model, KCL, CLPS, stride 0xc). One array would retarget
 *   those relocations. g_profile_YAJIRUSI_R/L live outside this TU.
 * Leftover: _ZN7Vector3D1Ev is vague linkage, licensed as a deadstrip
 *   duplicate of arm9 0x020072c0. This TU does not rehome it.
 */

#include "daObjYajirusi_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Player.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"

/* One column of the table at 0x0213c380. Stride 0xc covers the other two
 * words of the same row; only `value` is read. */
struct ArrowSignFileColumn {
    void *value;
    void *pad[2];
};

extern "C" {
extern void Matrix4x3_FromRotationY(void *mtx, int angle);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
int func_02012694(int id, void *pos);
int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void *self, short a, short b, short c, int d);
void func_02039394(int *collider, int v);
void func_020393a4(int *collider, int v);
void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mtx, int sy, int sx, int sz, unsigned int flags);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int radius, int unused);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, short angle, void *clps);
/* Model, KCL and CLPS. Each column is its own symbol so the relocations
 * stay on 0x0213c380 / 0x0213c384 / 0x0213c388. */
extern ArrowSignFileColumn data_ov098_0213c380[];
extern ArrowSignFileColumn data_ov098_0213c384[];
extern ArrowSignFileColumn data_ov098_0213c388[];
}

// 0x02138008 (0x38)
// @symbol daObjYajirusi_c_classInit_YAJIRUSI_L
/* Second registry factory for the same class (YAJIRUSI_L profile); body twin
 * of _R below. Contiguous at 0x02138008..0x02138040, so the genuine TU spans
 * 0x02137be0..0x02138040 with no hole. Historical alias: ArrowSignLeft_Spawn. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daObjYajirusi_c_classInit_YAJIRUSI_L(void)
{
    return (int *)new daObjYajirusi_c;
}
}

// 0x02137fd0 (0x38)
// @symbol daObjYajirusi_c_classInit_YAJIRUSI_R
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daObjYajirusi_c_classInit_YAJIRUSI_R(void)
{
    return (int *)new daObjYajirusi_c;
}
}

// 0x02137eec (0xe4)
int daObjYajirusi_c::InitResources()
{
    /* 0x12b YAJIRUSI_L -> column 0, 0x12c YAJIRUSI_R -> column 1.
       Any other id leaves mVariant untouched. */
    u16 id = actorID;
    if (id != 0x12b) {
        if (id == 0x12c)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    u32 modelIndex = mVariant;
    void *model = Model::LoadFile(*(SharedFilePtr *)data_ov098_0213c380[modelIndex].value);
    mModel.ModelBase::SetFile((BMD_File *)model, 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    func_ov098_02137c8c();

    u32 collisionIndex = mVariant;
    void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov098_0213c384[collisionIndex].value);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        data_ov098_0213c388[collisionIndex].value);
    return 1;
}

// 0x02137e48 (0xa4)
// @symbol _ZN15daObjYajirusi_c8BehaviorEv
int daObjYajirusi_c::Behavior()
{
    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(this, -0x2000, 0, 0, 0x96000))
        return 1;
    func_02039394((int *)&mMeshCollider, 0xc0000);
    func_020393a4((int *)&mMeshCollider, 0xe0000);
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat, 0x10e000, 0x64000, 0x46000, 0xf);
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x600000, 0);
    return 1;
}

// 0x02137e20 (0x28)
// @symbol _ZN15daObjYajirusi_c6RenderEv
int daObjYajirusi_c::Render()
{
    mModel.Render(0);
    return 1;
}

// 0x02137dbc (0x64)
// @symbol _ZN15daObjYajirusi_c16CleanupResourcesEv
int daObjYajirusi_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov098_0213c380[mVariant].value)->Release();
    ((SharedFilePtr *)data_ov098_0213c384[mVariant].value)->Release();
    return 1;
}

// 0x02137d80 (0x3c)
// @symbol _ZN15daObjYajirusi_c15OnHitByMegaCharER6Player
void daObjYajirusi_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePosX);
    KillByMegaChar(player);
}

// 0x02137d40 (0x40)
// @symbol _ZN15daObjYajirusi_c11OnAttacked1ER8dActor_c
/* Slot 22. A Bob-omb (actorID 0xce) breaks the sign. The slot's type is
 * int, and neither path returns a value. */
int daObjYajirusi_c::OnAttacked1(dActor_c &other)
{
    unsigned r = (other.actorID == 0xce) ? 1u : 0u;
    if (r == 0)
        return;
    Kill();
}

// 0x02137ccc (0x74)
// @symbol _ZN15daObjYajirusi_c4KillEv
/* Kill is vtable slot 31: _ZTV15daObjYajirusi_c (0x0213c3d8) relocates +0x7c to
 * 0x02137ccc where _ZTV10dBgActor_c carries dBgActor_c::Kill, so this is the
 * class's own override. Slot 30 is the same main-module function in both
 * tables, which makes 31 the first slot where they differ.
 *
 * Same shape as dBgActor_c::Kill with three differences the ROM dictates:
 * particle 0xe instead of 0xa, spawned 0x28000 (forty 20.12 units) above the
 * sign instead of a hundred, and DisappearPoofDustAt (particles 0x127/0x128)
 * instead of PoofDustAt.
 *
 * The second Vector3 is copied memberwise on purpose: Vector3 declares a
 * destructor (types.h), so a whole-object assignment compiles to an ldm/stm
 * pair, four instructions where the ROM has six. NewSimple keeps its mangled
 * name because its Fix12<int> parameters are by value and declaring the true
 * types changes how the caller passes them; dBgActor_c.cpp argues both. */
void daObjYajirusi_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x28000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xe, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    DisappearPoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}

// 0x02137c8c (0x40)
// @symbol _ZN15daObjYajirusi_c19func_ov098_02137c8cEv
/* Shadow matrix at mShadowMat: rotation from mAngleY, translation
 * mPos >> 3. The address is the name. */
void daObjYajirusi_c::func_ov098_02137c8c()
{
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    *(int *)((char *)&mShadowMat + 0x24) = mPosX >> 3;
    *(int *)((char *)&mShadowMat + 0x28) = mPosY >> 3;
    *(int *)((char *)&mShadowMat + 0x2c) = mPosZ >> 3;
}

// 0x02137c2c (0x60), 0x02137be0 (0x4c)
// @symbol _ZN15daObjYajirusi_cD0Ev
// @symbol _ZN15daObjYajirusi_cD1Ev
/* The destructor is defined inline-first in daObjYajirusi_c.h, which is what
 * makes mwccarm emit the retail D1-then-D0 pair in ROM order with the vtable
 * homed in this TU and no leaf D2 (class-form skill). Its body is the empty
 * braces plus the implicit member/base destruction the ROM's own D1/D0 show:
 * this class adds no member with a destructor of its own; D0 additionally
 * destroys through the base and returns the object to its heap via an inline
 * operator delete. */
