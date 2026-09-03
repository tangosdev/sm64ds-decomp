//cpp
/* Genuine-TU reconstruction for ov009/daMcFlag_c.
 *
 * The cartridge names this class itself: _ZTS10daMcFlag_c at ov009 0x02113b6c
 * reads "10daMcFlag_c", and the __si_class_type_info record at 0x02113b60
 * names dActor_c (_ZTI8dActor_c, arm9 0x0208e390) as the direct and only
 * base.  Flag was the project's coined name and is gone from the sources;
 * daMcFlag_c_Spawn and daMcFlag_c_SpawnInfo are convention spellings for two
 * derived C symbols the ROM says nothing about (historical aliases:
 * Flag_Spawn, Flag_SpawnInfo).
 *
 * mwccarm emits one .text section per function in the REVERSE of source
 * order, so this file is written highest-ROM-address first.  Do not reorder.
 *
 * InitResources is the key function, and the destructor is declared INLINE in
 * include/daMcFlag_c.h.  That pairing is what makes this range link: an
 * out-of-line destructor makes mwccarm emit D2, D0, D1, while the cartridge
 * has D1 at 0x02112078 then D0 at 0x021120a8 and no D2 at all, and production
 * isolation places .text sections into the spanning delink in emission order.
 * Inline, the compiler emits the two retail variants in the ROM's order,
 * together with this class's vtable and RTTI.
 *
 * Absorbed from these legacy one-function sources (ROM address order):
 *   [0] 0x02112078  src/_ZN10daMcFlag_cD1Ev.cpp
 *   [1] 0x021120a8  src/_ZN10daMcFlag_cD0Ev.cpp
 *   [2] 0x021120ec  src/_ZN10daMcFlag_c16CleanupResourcesEv.cpp
 *   [3] 0x0211211c  src/_ZN10daMcFlag_c6RenderEv.cpp
 *   [4] 0x02112144  src/_ZN10daMcFlag_c8BehaviorEv.cpp
 *   [5] 0x02112190  src/_ZN10daMcFlag_c13InitResourcesEv.cpp
 *   [6] 0x021121f0  src/daMcFlag_c_Spawn.c
 */

#include "daMcFlag_c.h"
#include "SharedFilePtr.h"

/* The two resource handles this class shares, both outside the .text range
 * this TU licenses: 0x02113eb8 is the BMD model file, 0x02113eb0 the BCA
 * animation.  Both are ov009 .bss, not .data -- config/arm9/overlays/ov009/
 * symbols.txt records them kind:bss -- and they are constructed at runtime by
 * a static initializer this repo already carries as source,
 * src/__sinit_ov009_02112ac8.c.  The two go through different constructors
 * there: 0x02113eb0 through _ZN13SharedFilePtr9ConstructEj (arm9 0x0201799c)
 * and 0x02113eb8 through func_02017acc, a same-sized sibling the symbol set
 * has not named yet.
 * This TU only loads and releases them; it neither defines nor initializes
 * them. */
extern "C" SharedFilePtr data_ov009_02113eb8;
extern "C" SharedFilePtr data_ov009_02113eb0;

/* Declared so daMcFlag_c_Spawn can spell the vptr store as &_ZTV[2].  That
   spelling is safe here for a reason worth stating exactly, because the
   obvious reading of it is wrong: this TU does emit the vtable (it is the
   key-function TU), but production isolation empties that emitted section and
   turns the symbol into an import -- in build/src/actors/d_a_mc_flag.o
   _ZTV/_ZTI/_ZTS10daMcFlag_c are all SHN_UNDEF, and the manifest's
   compiler_only_output rows are what re-supply the cartridge's copies.  The
   relocation is therefore EXTERNAL, not internal.  It still lands because
   mwccarm folds the 8-byte address-point bias at compile time: all three
   R_ARM_ABS32 relocations naming _ZTV10daMcFlag_c (the two destructor vptr
   stores and Spawn's) carry addend 0 with a zero content word, so each
   resolves to symbols.txt's 0x02113ba0 -- which IS the address point
   (0x02113b98 = 0 offset-to-top, 0x02113b9c = _ZTI, 0x02113ba0 = slot 0,
   InitResources).  The bias is computed, never patched; see
   notes/mwccarm-codegen.md and the mangled-body siblings, which reach the same
   addend-0 relocation by naming the bare symbol instead. */
extern int _ZTV10daMcFlag_c[];


extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(ModelAnim *);
extern void Matrix4x3_FromRotationY(void *, s16);

/* ModelAnim::SetAnim, spelled as the mangled free function it is defined as,
   with the speed argument declared SCALAR.  Its real signature takes
   Fix12<int> by value (include/ModelAnim.h line 88), and a by-value class
   argument makes mwccarm home the value and reload it with `ldr r3,[pc]; ldm
   r3,{r3}` -- two instructions and a literal-pool word the cartridge does not
   have; retail materialises the constant straight into r3 with `mov r3,
   #0x1000`.  That is wall 6az in notes/mwccarm-codegen.md, measured here
   through every aggregate form: brace-initialised local, const local, a
   returning inline helper, a gccext compound literal, and a Fix12 with a real
   raw-bits constructor all produce the same reload.  The same workaround is
   already carried by the landed src/actors/d_a_obj_bc_switch.cpp for
   Sound::ChangeMusicVolume and dBgActor_c::IsClsnInRange.  It costs
   InitResources the CONVERTED tier's no-mangled-refs criterion; that member
   did not hold the tier before the promotion either, so the ratchet moved
   2584 -> 2584, +0/-0, and no backslide exception was needed. */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self,
                                                        BCA_File *animFile,
                                                        int flags, int speed,
                                                        u32 startFrame);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daMcFlag_c_Spawn, 0x021121f0, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daMcFlag_c_Spawn
extern "C" daMcFlag_c *daMcFlag_c_Spawn(void)
{
    daMcFlag_c *actor = (daMcFlag_c *)_ZN7fBase_cnwEj(sizeof(daMcFlag_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV10daMcFlag_c[2];
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
    }
    return actor;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN10daMcFlag_c13InitResourcesEv, 0x02112190, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c13InitResourcesEv
s32 daMcFlag_c::InitResources()
{
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov009_02113eb8), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)Animation::LoadFile(data_ov009_02113eb0), 0,
        0x1000, 0);   /* 0x1000 = 1.0 in 20.12 */
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN10daMcFlag_c8BehaviorEv, 0x02112144, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c8BehaviorEv
s32 daMcFlag_c::Behavior()
{
    mModelAnim.Advance();
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.t.x = mPosX >> 3;
    mModelAnim.mat4x3.t.y = mPosY >> 3;
    mModelAnim.mat4x3.t.z = mPosZ >> 3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN10daMcFlag_c6RenderEv, 0x0211211c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c6RenderEv
s32 daMcFlag_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN10daMcFlag_c16CleanupResourcesEv, 0x021120ec, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c16CleanupResourcesEv
s32 daMcFlag_c::CleanupResources()
{
    data_ov009_02113eb8.Release();
    data_ov009_02113eb0.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- _ZN10daMcFlag_cD0Ev 0x021120a8, _ZN10daMcFlag_cD1Ev
 * 0x02112078.  No definition here on purpose: the inline `~daMcFlag_c() {}` in
 * include/daMcFlag_c.h emits both variants, in the ROM's D1-then-D0 order, and
 * the deallocation D0 performs is dActor_c's inline operator delete.  An
 * inline {} body is not an empty destructor -- mModelAnim has one, which is
 * why D1 is 0x30 bytes and not a bare return.
 * -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_cD0Ev
// @symbol _ZN10daMcFlag_cD1Ev
