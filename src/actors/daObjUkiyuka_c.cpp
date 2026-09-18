//cpp
/* The floating floors' shared base -- ov002/daObjUkiyuka_c.
 *
 * The class is abstract in slots 0 (InitResources) and 3 (CleanupResources), so
 * its setup and teardown are not vtable slots at all: each of the two leaves
 * calls func_ov002_020b6584 and func_ov002_020b6424 below with its own file
 * table. Behavior consumes what the setup half seeded -- the rest height at
 * 0x320 and the bob amplitude at 0x324. include/daObjUkiyuka_c.h carries the
 * class banner.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here.
 *
 * Folded from four one-function sources, which this promotion deletes. They are
 * listed by the symbol each one carried, in ROM address order:
 *   [0] 0x020b6424  func_ov002_020b6424
 *   [1] 0x020b646c  _ZN14daObjUkiyuka_c6RenderEv
 *   [2] 0x020b6494  _ZN14daObjUkiyuka_c8BehaviorEv
 *   [3] 0x020b6584  func_ov002_020b6584
 *
 * THE DESTRUCTOR PAIR IS NOT CLAIMED. _ZN14daObjUkiyuka_cD0Ev (0x020b6388) and
 * _ZN14daObjUkiyuka_cD1Ev (0x020b63e0) sit immediately below this range and
 * keep their own delinks entries. This TU emits both anyway -- Behavior is the
 * class's key function, so _ZTV14daObjUkiyuka_c is anchored here and its slots
 * 16 and 17 odr-use them -- but the cartridge orders them D0-then-D1 and
 * mwccarm emits the vague-linkage pair of an inline-in-class destructor
 * D1-then-D0 in every source form measured on THIS TU. The manifest's
 * boundary_evidence records those compiles and why the one form that would
 * emit D0 first is refuted by the ROM. Nothing here forces or reorders them;
 * the copies are licensed as deadstrip-duplicate against their ROM homes.
 */

#include "daObjUkiyuka_c.h"

extern "C" {
extern int _ZN4dBgW9IsEnabledEv(void *);
extern void _ZN4dBgW7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern s16 data_02082214[];
u16 DecIfAbove0_Short(u16 *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020b6584, 0x020b6584, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6584
/* The shared resource setup every daObjUkiyuka_c leaf calls: load the model
 * file into dBgActor_c's Model at 0xd4, place it, load the collision file into
 * the dBgW_KcMbg at 0x124 with the leaf's CLPS block, then seed the two fields
 * this class adds -- the rest height at 0x320 from the actor's current mPosY,
 * and the bob amplitude at 0x324 from the caller's argument. Those two stores
 * are also what fixes this function to THIS class rather than to the
 * neighbouring daObjKaitendai_c, whose sizeof is 0x320 exactly; see the
 * manifest's boundary_evidence.
 *
 * Reached only through the two leaves' own InitResources (ov022 0x021120a4 as
 * a direct call, ov045 0x02111bec through a literal-pool load), so the
 * address-derived name and the void * signature stay: nothing in the ROM names
 * it and no vtable slot holds it.
 *
 * SPELLINGS ARE THE PRE-FOLD ONES, deliberately. This body arrives from the
 * pre-fold shard for 0x020b6584 unchanged apart from one declaration: it
 * spelled parameter 4 through a file-local `typedef int Fix12`, and `Fix12` is
 * a class template once the header is in scope, so the parameter is spelled
 * `int` -- the type the typedef named -- directly. Keeping the rest as it was
 * is what makes this file's rows in config/decl-agreement-baseline.json a
 * re-key rather than a new disagreement. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
int func_ov002_020b6584(void* c, void** f, int a) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(f[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, mdl, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(f[1]);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, kcl, (char*)c+0x2ec, 0x1000, *(short*)((char*)c+0x8e), f[2]);
  *(int*)((char*)c+0x320) = *(int*)((char*)c+0x60);
  *(int*)((char*)c+0x324) = a;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN14daObjUkiyuka_c8BehaviorEv, 0x020b6494, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjUkiyuka_c8BehaviorEv
/* daObjUkiyuka_c::Behavior -- vtable slot 6, ov002 0x020b6494.
 *
 * Attributed by the vtable, not by a `recovered name:` comment -- the
 * pre-migration file carried none at all, only a `char *c` and raw offsets.
 * daObjUkiyuka_c's own vtable (ov002 0x0210912c) differs from dBgActor_c's in
 * slots 6, 9, 16 and 17; this address is slot 6, so it is this class's own
 * override and not either leaf's. include/daObjUkiyuka_c.h's banner already
 * names the four fields this body reads from exactly this function.
 *
 * THE FIELD SPELLINGS ARE THE PRE-MIGRATION ONES, deliberately. mBobPhase is
 * declared s16 but is stepped as a u16 and then sign-read as an s16 in the
 * same breath; mRestTimer is passed to DecIfAbove0_Short by address. Writing
 * either through the member rather than through the cast changes what mwccarm
 * CSEs -- see notes/mwccarm-codegen.md on compound assignment -- so the two
 * that matter keep the `(int)this + off` launder and the rest read as members.
 *
 * dBgActor_c::IsClsnInRange is NOT declared in include/dBgActor_c.h: its ROM
 * name carries by-value Fix12<int> parameters, which mwccarm passes
 * differently from the `int` this call site needs, so it stays an extern "C"
 * of the mangled symbol exactly as every other caller in the tree spells it. */
s32 daObjUkiyuka_c::Behavior()
{
    char *c = (char *)this;

    if (DecIfAbove0_Short(&mRestTimer) != 0) {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
        return 1;
    }
    *(u16 *)((int)c + 0x328) += 0x100;
    {
        u16 h = (u16)((s64)mBobPhase);
        *(int *)((int)c + 0x60) -=
            (int)((((s64)mBobAmplitude * data_02082214[(h >> 4) * 2]) + 0x800) >> 0xc);
    }
    {
        int d = mPosY - mRestY;
        if (d < 0)
            d = -d;
        if (d == 0)
            mRestTimer = 0x3c;
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjUkiyuka_c6RenderEv, 0x020b646c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjUkiyuka_c6RenderEv
/* daObjUkiyuka_c::Render -- vtable slot 9, ov002 0x020b646c.
 * Behavior (slot 6) already holds the key function for this class, so
 * declaring Render after it is layout- and key-function-neutral. Render
 * dispatches through dBgActor_c's own mModel (0xd4), as every sibling in
 * this series does. */
s32 daObjUkiyuka_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- func_ov002_020b6424, 0x020b6424, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6424
/* The teardown half of the pair above: disable the mesh collider if it is
 * live, then release the leaf's two SharedFilePtrs. Reached only through a
 * literal-pool load in each of the two leaves (ov022 0x0211203c,
 * ov045 0x02111bd4). Instruction-for-instruction the same body
 * daObjGuragura_c, daObjKaitendai_c, daObjKuruma_c and daObjKurumajiku_c each
 * carry their own copy of, which is why each copy sits inside its own class's
 * run rather than being shared. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020b6424(char *t, void **f){
  if(_ZN4dBgW9IsEnabledEv(t+0x124))
    _ZN4dBgW7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(f[0]);
  _ZN13SharedFilePtr7ReleaseEv(f[1]);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* BELOW THIS RANGE, NOT CLAIMED -- _ZN14daObjUkiyuka_cD1Ev (0x020b63e0) and
 * _ZN14daObjUkiyuka_cD0Ev (0x020b6388).
 *
 * NOT WRITTEN OUT. ~daObjUkiyuka_c is defined inline in the class body --
 * include/daObjUkiyuka_c.h records why: both descendants inline its vptr store
 * instead of calling it, which the compiler can only do from a visible body.
 * Behavior above is this class's key function, so this TU emits
 * _ZTV14daObjUkiyuka_c and its slots 16 and 17 odr-use the two variants
 * anyway. Their emission ORDER is the compiler's, not the source's, and it
 * does not agree with the cartridge; see the file banner.
 * -------------------------------------------------------------------------- */
