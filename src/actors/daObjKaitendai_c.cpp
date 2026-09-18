//cpp
/* The turntable platforms' shared base -- ov002/daObjKaitendai_c.
 *
 * The class is abstract in slots 0 (InitResources) and 3 (CleanupResources), so
 * its setup and teardown are not vtable slots at all: each of the five leaves
 * calls func_ov002_020b676c and func_ov002_020b66a8 below with its own file
 * table. Behavior consumes what the setup half seeded. include/daObjKaitendai_c.h
 * carries the class banner.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here.
 *
 * Folded from four one-function sources, which this promotion deletes. They are
 * listed by the symbol each one carried, in ROM address order:
 *   [0] 0x020b66a8  func_ov002_020b66a8
 *   [1] 0x020b66f0  _ZN16daObjKaitendai_c6RenderEv
 *   [2] 0x020b6718  _ZN16daObjKaitendai_c8BehaviorEv
 *   [3] 0x020b676c  func_ov002_020b676c
 *
 * THE DESTRUCTOR PAIR IS NOT CLAIMED. _ZN16daObjKaitendai_cD0Ev (0x020b660c) and
 * _ZN16daObjKaitendai_cD1Ev (0x020b6664) sit immediately below this range and
 * keep their own delinks entries. This TU emits both anyway -- Behavior is the
 * class's key function, so _ZTV16daObjKaitendai_c is anchored here and its slots
 * 16 and 17 odr-use them -- but the cartridge orders them D0-then-D1 and mwccarm
 * emits the vague-linkage pair of an inline-in-class destructor D1-then-D0 in
 * every source form measured. The manifest's boundary_evidence records the four
 * compiles and why the one form that would emit D0 first is refuted by the ROM.
 * Nothing here forces or reorders them; the copies are licensed as
 * deadstrip-duplicate against their ROM homes.
 */

#include "daObjKaitendai_c.h"

extern "C" {
extern int _ZN4dBgW9IsEnabledEv(void *);
extern void _ZN4dBgW7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *c, int a, int b);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020b676c, 0x020b676c, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b676c
/* The shared resource setup every daObjKaitendai_c leaf calls: load the model
 * file into dBgActor_c's Model at 0xd4, place it, load the collision file into
 * the dBgW_KcMbg at 0x124 with the leaf's CLPS block, then seed the per-frame
 * spin. The spin lands at 0x96 -- mPrevAngleZ, the field this TU's own Behavior
 * consumes -- so the two halves of one class sit on either side of Render.
 *
 * Reached only through a literal-pool load in each of the five leaves
 * (ov015 0x02112cb0, ov022 0x02111680, ov029 0x02112160, ov036 0x0211153c,
 * ov047 0x021113b4), so the address-derived name and the unsigned char *
 * signature stay: nothing in the ROM names it and no vtable slot holds it. */
/* The leaf's file table, spelled the way the four callers that survive as their
 * own sources spell it. The tag is deliberately NOT renamed: of the five leaves
 * that call this function, src/_ZN19RotatingPlatformLll13InitResourcesEv.cpp
 * (ov022) is the one whose declaration currently AGREES with this definition,
 * and any other tag here would break that agreement and bank a new row in
 * config/decl-agreement-baseline.json for a name the compiler never sees --
 * the call is extern "C", so the tag reaches no mangled name and no codegen. */
struct Arg { struct SharedFilePtr *m[2]; struct CLPS_Block *clps; };

extern "C" {
struct BMD_File;
struct KCL_File;
struct CLPS_Block;
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
void func_020393d4(int *p, int v);

int func_ov002_020b676c(unsigned char *self, struct Arg *a, short arg2)
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*a->m[0]);
    short y;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*a->m[1]);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, kcl, *(struct Matrix4x3 *)(self + 0x2ec), 0x199,
            *(s16 *)(self + 0x8e), *a->clps);
    }
    func_020393d4((int *)(self + 0x124), (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    *(s16 *)(self + 0x96) = arg2;
    y = *(s16 *)(self + 0x90);
    if (y != 0) {
        *(s16 *)(self + 0x96) = y;
        *(s16 *)(self + 0x90) = 0;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjKaitendai_c8BehaviorEv, 0x020b6718, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjKaitendai_c8BehaviorEv
/* daObjKaitendai_c::Behavior -- vtable slot 6, ov002 0x020b6718.
 *
 * Attributed by the vtable, not by a `recovered name:` comment -- the
 * pre-migration file carried none. daObjKaitendai_c's own vtable (ov002
 * 0x021091d4) differs from dBgActor_c's in slots 6, 9, 16 and 17 only, and this
 * address is slot 6, so the function belongs to this class and not to any of
 * its five leaves, which merely inherit it. include/daObjKaitendai_c.h's
 * banner already reads this exact function to conclude the class has no
 * fields: everything it touches is dActor_c's.
 *
 * The turntable step itself: advance the spin by the per-frame increment held
 * in mPrevAngleZ, then publish it as the actor's facing angle. The two
 * dBgActor_c helpers are non-virtual, so an unqualified call is already the
 * direct `bl` the ROM has.
 *
 * THE `s16 *p` IS LOAD-BEARING, not a leftover. `mPrevAngleY += mPrevAngleZ;`
 * recomputes the address for the store and comes out 0x50 against the ROM's
 * 0x54; taking the address once is what makes mwccarm CSE it into a register,
 * which is what the cartridge does. Same lever notes/mwccarm-codegen.md
 * records for compound assignment, arrived at from the other side.
 *
 * dBgActor_c::IsClsnInRangeOnScreen is NOT declared in include/dBgActor_c.h: its
 * ROM name carries by-value Fix12<int> parameters, which mwccarm passes
 * differently from the `int` this call site needs, so it stays an extern "C"
 * of the mangled symbol, the way every other caller in the tree spells it. */
s32 daObjKaitendai_c::Behavior()
{
    s16 *p = &mPrevAngleY;
    *p = *p + mPrevAngleZ;
    mAngleY = mPrevAngleY;
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjKaitendai_c6RenderEv, 0x020b66f0, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjKaitendai_c6RenderEv
/* daObjKaitendai_c::Render -- vtable slot 9, ov002 0x020b66f0.
 * Behavior (slot 6) already holds the key function for this class, so
 * declaring Render after it is layout- and key-function-neutral. Render
 * dispatches through dBgActor_c's own mModel (0xd4), as every sibling in
 * this series does. */
s32 daObjKaitendai_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- func_ov002_020b66a8, 0x020b66a8, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b66a8
/* The teardown half of the pair above: disable the mesh collider if it is
 * live, then release the leaf's two SharedFilePtrs. Reached only through a
 * literal-pool load in each of the same five leaves (ov015 0x02112c90,
 * ov022 0x02111668, ov029 0x02112140, ov036 0x02111504, ov047 0x0211137c).
 * Instruction-for-instruction the same body daObjGuragura_c, daObjUkiyuka_c,
 * daObjKuruma_c and daObjKurumajiku_c each carry their own copy of, which is
 * why each copy sits inside its own class's run rather than being shared. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020b66a8(char *t, void **f){
  if(_ZN4dBgW9IsEnabledEv(t+0x124))
    _ZN4dBgW7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(f[0]);
  _ZN13SharedFilePtr7ReleaseEv(f[1]);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* BELOW THIS RANGE, NOT CLAIMED -- _ZN16daObjKaitendai_cD1Ev (0x020b6664) and
 * _ZN16daObjKaitendai_cD0Ev (0x020b660c).
 *
 * NOT WRITTEN OUT. ~daObjKaitendai_c is defined inline in the class body --
 * include/daObjKaitendai_c.h records why: all five descendants inline its vptr
 * store instead of calling it, which the compiler can only do from a visible
 * body. Behavior above is this class's key function, so this TU emits
 * _ZTV16daObjKaitendai_c and its slots 16 and 17 odr-use the two variants
 * anyway. Their emission ORDER is the compiler's, not the source's, and it does
 * not agree with the cartridge; see the file banner.
 * -------------------------------------------------------------------------- */
