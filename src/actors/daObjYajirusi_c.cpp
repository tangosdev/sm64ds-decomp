//cpp
/**
 * Arrow sign (YAJIRUSI_R / YAJIRUSI_L).
 *
 * Signpost with mesh collision. The two registry variants select the model
 * and collision columns through mVariant (actor id 0x12b/0x12c); Kill and
 * the Mega handlers break it with a particle poof.
 *
 * daObjYajirusi_c_classInit_YAJIRUSI_R/_L are reconstructed (RTTI
 * daObjYajirusi_c at 0x0213c36c, YAJIRUSI_R/L registry). Retail does not
 * store those spellings. Historical aliases ArrowSignRight_Spawn /
 * ArrowSignLeft_Spawn.
 *
 * deslop
 * Leftover: function order is reverse ROM (highest address first) --
 *   mwccarm 2004/b56 emits one .text section per function in reverse
 *   source order. Do not reorder; the D1/D0 pair is in ROM order
 *   because the destructor is inline-first in daObjYajirusi_c.h and the
 *   factories below are real new-expressions (class-form skill).
 * Leftover: twin _L factory folded by hand (0x02138008..0x02138040):
 *   identical body to _R (alloc 896, dBgActor_c C2, same vtable,
 *   ShadowModel C1 at +0x320), contiguous with no gap -- one genuine TU,
 *   0x02137be0..0x02138040, 12 functions.
 * Leftover: func_ov098_02137c8c keeps its ROM-unnamed spelling; it builds
 *   the collision/model matrix at +0x348/0x36c from yaw and pos>>3 in
 *   InitResources. No replacement is coined.
 * Leftover: factories are `return (int *)new daObjYajirusi_c` -- real
 *   instantiation, which is what makes mwccarm emit the D1-then-D0 pair
 *   in ROM order plus the vtable homed here. The synthesized constructor
 *   reproduces the ROM init sequence (alloc 896, dBgActor_c C2, vptr,
 *   ShadowModel C1 at +0x320) byte-exact; the hand-rolled spelling is
 *   gone. The inline chain also emits a homeless _ZN10dBgActor_cD2Ev,
 *   licensed deadstrip in the manifest (ov012/daObjC0Water_c precedent).
 * Leftover: OnAttacked1/OnHitByMegaChar reference spellings are coined
 *   guesses (ref vs pointer is indistinguishable in ARM); the method names
 *   are vtable-slot recovered and ownership/bodies/relocations are proven.
 *   Disclosed in the manifest notes, symbols/actor_renames.tsv, and
 *   daObjYajirusi_c.h.
 * Leftover: S14 g_profile_YAJIRUSI_R/L stay outside the TU.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjYajirusi_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Player.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"

/* This class's own field access is real member syntax throughout (mModel,
 * mMeshCollider, mVariant -- daObjYajirusi_c.h); no shadow struct stands in for
 * a real project class. The one local type below is genuine ROM-table
 * recovery, not a workaround. */
/* shadow struct 'ArrowSignFileColumn' */
struct ArrowSignFileColumn {
    void *value;
    void *nextColumn1;
    void *nextColumn2;
};

extern "C" {
extern void Matrix4x3_FromRotationY(void *, int);
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_( u32 id, Fix12i x, Fix12i y, Fix12i z);
int func_02012694(int, void*);
int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void* c, short a, short b, short d, int e);
void func_02039394(int* p, int v);
void func_020393a4(int* p, int v);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void* c, void* sm, void* mtx, int s, int x, int y, unsigned int j);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);
void func_ov098_02137c8c(char *self);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
void *self, void *kcl, void *mtx, int scale, short angle, void *clps);
/* Each row is model/KCL/CLPS, but the ROM gives each column its own symbol.
* These three overlapping stride-0xc views keep those relocation destinations
* distinct while still indexing the table as rows. */
extern ArrowSignFileColumn data_ov098_0213c380[];
extern ArrowSignFileColumn data_ov098_0213c384[];
extern ArrowSignFileColumn data_ov098_0213c388[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- daObjYajirusi_c_classInit_YAJIRUSI_L, 0x02138008, size 0x38 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- daObjYajirusi_c_classInit_YAJIRUSI_R, 0x02137fd0, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daObjYajirusi_c_classInit_YAJIRUSI_R
/* recovered: vtable identified, globals resolved, declarations from a shared header */
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV15daObjYajirusi_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daObjYajirusi_c_classInit_YAJIRUSI_R(void)
{
    return (int *)new daObjYajirusi_c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN15daObjYajirusi_c13InitResourcesEv, 0x02137eec, size 0xe4 */
/* -------------------------------------------------------------------------- */
int daObjYajirusi_c::InitResources()
{
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
    func_ov098_02137c8c((char *)this);

    u32 collisionIndex = mVariant;
    void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov098_0213c384[collisionIndex].value);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        data_ov098_0213c388[collisionIndex].value);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN15daObjYajirusi_c8BehaviorEv, 0x02137e48, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjYajirusi_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
int daObjYajirusi_c::Behavior()
{
    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(((char*)this), -0x2000, 0, 0, 0x96000))
        return 1;
    func_02039394((int*)((char*)&(*(u8 *)&mMeshCollider)), 0xc0000);
    func_020393a4((int*)((char*)&(*(u8 *)&mMeshCollider)), 0xe0000);
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        ((char*)this), (void*)((char*)&mShadowModel), (void*)((char*)&mShadowMat), 0x10e000, 0x64000, 0x46000, 0xf);
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(((char*)this), 0x600000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN15daObjYajirusi_c6RenderEv, 0x02137e20, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjYajirusi_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daObjYajirusi_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN15daObjYajirusi_c16CleanupResourcesEv, 0x02137dbc, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjYajirusi_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjYajirusi_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov098_0213c380[mVariant].value)->Release();
    ((SharedFilePtr *)data_ov098_0213c384[mVariant].value)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN15daObjYajirusi_c15OnHitByMegaCharER6Player, 0x02137d80, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjYajirusi_c15OnHitByMegaCharER6Player
// recovered name: ArrowSignRight_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* daObjYajirusi_c::OnHitByMegaChar - recovered from vtable slot identity */
void daObjYajirusi_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1e, (char*)this + 0x74);
    KillByMegaChar(player);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN15daObjYajirusi_c11OnAttacked1ER8dActor_c, 0x02137d40, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjYajirusi_c11OnAttacked1ER8dActor_c
// recovered name: ArrowSignRight_OnAttacked1
/* recovered: renamed to Class_Method */
/* daObjYajirusi_c::OnAttacked1 - recovered from vtable slot identity. The
   draft indexed a raw 64-entry function-pointer vtable at word 0x7c/4 =
   31; that slot is Kill(), already a named virtual on this class, so
   this calls it directly. `other`'s actorID (offset 0xc) has no public
   C++ name on dActor_c (only the C-fallback flat struct names it), so it
   stays a raw offset read, matching every other sibling override in this
   tree that reads another actor's fields this way. */
int daObjYajirusi_c::OnAttacked1(dActor_c &other)
{
    unsigned r = (*(unsigned short*)((char*)&other + 0xc) == 0xce) ? 1u : 0u;
    if (r == 0) return;
    Kill();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN15daObjYajirusi_c4KillEv, 0x02137ccc, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjYajirusi_c4KillEv
/* daObjYajirusi_c::Kill() at ov098 0x02137ccc, 0x74 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV15daObjYajirusi_c (ov098 0x0213c3d8) relocates
 * its word at +0x7c to 0x02137ccc, and _ZTV10dBgActor_c carries
 * _ZN10dBgActor_c4KillEv at the same slot, so this is this class's own override of
 * dBgActor_c's Kill. Slot 30 (+0x78) is the main-module 0x02010124 in both
 * tables, which is what makes 31 the first slot where they differ.
 *
 * The file used to carry `// recovered name: ArrowSignRight_Kill` alongside
 * `daObjYajirusi_c::Kill`. daObjYajirusi_c IS this class's RTTI name, so the
 * two agreed here; the comment is gone anyway because the vtable, not a
 * comment, is the source.
 *
 * Same shape as dBgActor_c::Kill, with three differences the ROM dictates: the
 * particle is 0xe rather than 0xa, it spawns 0x28000 -- forty 20.12 units --
 * above the sign rather than a hundred, and the poof is DisappearPoofDustAt
 * (particles 0x127/0x128) rather than PoofDustAt.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. Particle::System::NewSimple stays spelled
 * as its mangled name -- its parameters are Fix12<int> BY VALUE and declaring
 * the true types changes how the caller passes them. Both points are argued in
 * full in src/actors/dBgActor_c.cpp. */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov098_02137c8c, 0x02137c8c, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov098_02137c8c(char *t)
{
    Matrix4x3_FromRotationY(t + 0x348, *(short *)(t + 0x8e));
    *(int *)(t + 0x36c) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x370) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x374) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN15daObjYajirusi_cD0Ev, 0x02137c2c, size 0x60 */
/* ROM ordinal 0 -- _ZN15daObjYajirusi_cD1Ev, 0x02137be0, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjYajirusi_cD0Ev
// @symbol _ZN15daObjYajirusi_cD1Ev
/* The destructor is defined inline-first in daObjYajirusi_c.h, which is what
 * makes mwccarm emit the retail D1-then-D0 pair in ROM order with the vtable
 * homed in this TU and no leaf D2 (class-form skill). Its body is the empty
 * braces plus the implicit member/base destruction the ROM's own D1/D0 show:
 * this class adds no member with a destructor of its own; D0 additionally
 * destroys through the base and returns the object to its heap via an inline
 * operator delete. */
