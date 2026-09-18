//cpp
/* The rickshaw platforms' shared base -- ov002/daObjKuruma_c.
 *
 * The class is abstract in slots 0 (InitResources) and 3 (CleanupResources), so
 * its setup and teardown are not vtable slots at all: each of the two leaves
 * calls func_ov002_020b6958 and func_ov002_020b68b0 below with its own file
 * table. include/daObjKuruma_c.h carries the class banner.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here.
 *
 * Folded from four one-function sources, which this promotion deletes. They are
 * listed by the symbol each one carried, in ROM address order:
 *   [0] 0x020b68b0  func_ov002_020b68b0
 *   [1] 0x020b68f8  _ZN13daObjKuruma_c6RenderEv
 *   [2] 0x020b6920  _ZN13daObjKuruma_c8BehaviorEv
 *   [3] 0x020b6958  func_ov002_020b6958
 *
 * THE DESTRUCTOR PAIR IS NOT CLAIMED. _ZN13daObjKuruma_cD0Ev (0x020b6814) and
 * _ZN13daObjKuruma_cD1Ev (0x020b686c) sit immediately below this range and keep
 * their own delinks entries. This TU emits both anyway -- Render is this class's
 * key function, so _ZTV13daObjKuruma_c is anchored here and its slots 16 and 17
 * odr-use them -- but the cartridge orders them D0-then-D1 and mwccarm emits the
 * vague-linkage pair of an inline-in-class destructor D1-then-D0 in every source
 * form measured on THIS TU. The manifest's boundary_evidence records those
 * compiles and why the forms that would emit D0 first are refuted by the ROM.
 * Nothing here forces or reorders them; the copies are licensed as
 * deadstrip-duplicate against their ROM homes.
 */

#include "daObjKuruma_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020b6958, 0x020b6958, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6958
/* The shared resource setup every daObjKuruma_c leaf calls: load the model file
 * into dBgActor_c's Model at 0xd4, place it, load the collision file into the
 * dBgW_KcMbg at 0x124 with the leaf's CLPS block, then bind the mesh collider's
 * per-frame update to dBgW::UpdatePosWithVelocity. Unlike the neighbouring
 * daObjKaitendai_c's setup half it seeds no field of its own and takes no third
 * argument -- this class adds no state, which include/daObjKuruma_c.h records
 * from the other side (both leaf factories pass 0x320 = sizeof(dBgActor_c) to
 * fBase_c::operator new).
 *
 * Reached only through a literal-pool load in each of the two leaves'
 * InitResources -- daObjKm1_Kuruma_c::InitResources (ov043 0x021115ec) and
 * daObjKm3_Kuruma_c::InitResources (ov047 0x021114cc) -- so the address-derived
 * name and the void * signature stay: nothing in the ROM names it and no vtable
 * slot holds it.
 *
 * SPELLINGS ARE THE PRE-FOLD ONES, deliberately. This body arrives from the
 * pre-fold shard for 0x020b6958 unchanged apart from one declaration: it spelled
 * parameter 4 of dBgW_KcMbg::SetFile through a file-local `typedef int Fix12`,
 * and `Fix12` is a class template once the header is in scope, so the parameter
 * is spelled `int` -- the type the typedef named -- directly. Keeping the rest
 * as it was is what makes this file's rows in config/decl-agreement-baseline.json
 * a re-key rather than a new disagreement. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int func_020393d4(void*, void*);
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
int func_ov002_020b6958(void* c, void** f) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(f[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, mdl, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(f[1]);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, kcl, (char*)c+0x2ec, 0x199, *(short*)((char*)c+0x8e), f[2]);
  func_020393d4((char*)c+0x124, (void*)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN13daObjKuruma_c8BehaviorEv, 0x020b6920, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjKuruma_c8BehaviorEv
/* daObjKuruma_c::Behavior -- vtable slot 6, ov002 0x020b6920.
 *
 * Attributed by the vtable, not by a `recovered name:` comment: the pre-fold
 * file carried none at all, only the func_ov002_ address. daObjKuruma_c's own
 * vtable (ov002 address point 0x02109278) differs from dBgActor_c's in slots 6,
 * 9, 16 and 17 only, and this address is slot 6, so the function belongs to this
 * class and not to either leaf, which merely inherit it.
 *
 * Two of the three calls are dBgActor_c's own non-virtual members, so they are
 * spelled as calls rather than through `extern "C"` declarations of the mangled
 * symbols -- the compiler emits the identical direct `bl` either way.
 * IsClsnInRange stays a mangled extern because include/dBgActor_c.h does not
 * declare it and that header belongs to the whole dBgActor_c family, not to this
 * slice; naming it there is its own change.
 *
 * `this` is not adjusted anywhere below: daObjKuruma_c has a single base at
 * subobject offset 0 (__si_class_type_info, per the banner), so the void* the
 * ROM passes in r0 and `this` are the same address. */
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *thiz, int x, int z);

s32 daObjKuruma_c::Behavior()
{
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN13daObjKuruma_c6RenderEv, 0x020b68f8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjKuruma_c6RenderEv
/* daObjKuruma_c::Render -- vtable slot 9, ov002 0x020b68f8.
 *
 * RENDER IS THIS CLASS'S KEY FUNCTION and it is now defined here, which is the
 * one thing this fold changes about the class's emission. The destructor is
 * inline in the class body, so the key function is the first non-inline virtual
 * DECLARED in the class, and include/daObjKuruma_c.h declares Render before
 * Behavior deliberately. Before this fold Render's definition was unmigrated, so
 * no TU owned the key function and nothing emitted _ZTV13daObjKuruma_c; that
 * header comment is updated by this promotion. This TU now emits the vtable and
 * the RTTI triple as compiler-only output, exactly as the two landed siblings in
 * this stretch of ov002 do, and objisolate drops them -- the ROM's copies at
 * ov002 0x02109278 / 0x02109254 / 0x02109260 are delinked data owned elsewhere.
 *
 * Render dispatches through dBgActor_c's own mModel (0xd4), as every sibling in
 * this series does. */
s32 daObjKuruma_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- func_ov002_020b68b0, 0x020b68b0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b68b0
/* The teardown half of the pair above: disable the mesh collider if it is live,
 * then release the leaf's two SharedFilePtrs. Reached only through a
 * literal-pool load in each of the two leaves' CleanupResources --
 * daObjKm1_Kuruma_c::CleanupResources (ov043 0x021115d8) and
 * daObjKm3_Kuruma_c::CleanupResources (ov047 0x021114b8).
 * Instruction-for-instruction the same body daObjGuragura_c, daObjUkiyuka_c,
 * daObjKaitendai_c and daObjKurumajiku_c each carry their own copy of, which is
 * why each copy sits inside its own class's run rather than being shared.
 *
 * SPELLING IS THE PRE-FOLD ONE. Unlike its four siblings' copies, this one was
 * already migrated to C++ and reaches dBgW and SharedFilePtr through their real
 * headers and real member calls instead of through extern "C" declarations of
 * the mangled names. That is why this file includes dBgW.h and SharedFilePtr.h
 * at the top; the body below is unchanged from that pre-fold shard. */
extern "C" {
int func_ov002_020b68b0(void* c, void* r4) {
    if (((dBgW *)((char*)c+0x124))->IsEnabled())
        ((dBgW *)((char*)c+0x124))->Disable();
    ((SharedFilePtr *)(*(void**)r4))->Release();
    ((SharedFilePtr *)(*(void**)((char*)r4+4)))->Release();
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* BELOW THIS RANGE, NOT CLAIMED -- _ZN13daObjKuruma_cD1Ev (0x020b686c) and
 * _ZN13daObjKuruma_cD0Ev (0x020b6814).
 *
 * NOT WRITTEN OUT. ~daObjKuruma_c is defined inline in the class body --
 * include/daObjKuruma_c.h records why: both descendants inline its vptr store
 * instead of calling it, which the compiler can only do from a visible body.
 * Render above is this class's key function, so this TU emits
 * _ZTV13daObjKuruma_c and its slots 16 and 17 odr-use the two variants anyway.
 * Their emission ORDER is the compiler's, not the source's, and it does not
 * agree with the cartridge; see the file banner.
 * -------------------------------------------------------------------------- */
