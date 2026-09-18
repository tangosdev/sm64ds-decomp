//cpp
/* ov002/daObjUkiyuka_c -- the abstract floating-floor base, as one reconstructed
 * translation unit. 5 functions, ROM 0x020b6388..0x020b6584.
 *
 * SHADOW, NOT ENROLLED. config/arm9/overlays/ov002/delinks.txt still names the
 * five legacy one-function sources, so this file contributes nothing to the ROM
 * build yet. It is text-verified, not promoted, and the reason is a destructor
 * EMISSION ORDER that no source form reachable from here produces -- measured
 * below, because the measurement is the useful part of this change.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020b6388  src/_ZN14daObjUkiyuka_cD0Ev.cpp
 *   [1] 0x020b63e0  src/_ZN14daObjUkiyuka_cD1Ev.cpp
 *   [2] 0x020b6424  src/func_ov002_020b6424.c
 *   [3] 0x020b646c  src/_ZN14daObjUkiyuka_c6RenderEv.cpp
 *   [4] 0x020b6494  src/_ZN14daObjUkiyuka_c8BehaviorEv.cpp
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST. Do not reorder.
 *
 * THE DESTRUCTOR IS NOT WRITTEN HERE. It is defined inline in the class body in
 * include/daObjUkiyuka_c.h, and this TU owns the class's key function
 * (Behavior, the first out-of-line virtual), so the compiler emits
 * _ZTV14daObjUkiyuka_c and, with it, the D1/D0 pair the vtable references. The
 * two legacy shards' `EmitDestructor` / `EmitDeletingDestructor` forcing
 * functions are therefore DELETED here rather than carried over: in a merged TU
 * the vtable already odr-uses both variants, and the forcers only add two
 * source-written .text sections that promotion cannot license.
 *
 * WHY THIS IS STILL text-verified AND NOT promoted.
 * The cartridge orders this class's variants D0 (0x020b6388, 0x58) BELOW D1
 * (0x020b63e0, 0x44), and they are the FIRST two functions in the range.
 * Production isolation places an object's .text sections into the spanning
 * delink in emission order, so a swapped pair byte-matches per function and
 * still fails the whole-range link. Measured on this TU, one compile each:
 *
 *   source form                                              emitted pair
 *   ------------------------------------------------------   ------------
 *   inline dtor, deferred codegen (default)                   D1, D0  front
 *   inline dtor, deferred, forcer order swapped               D1, D0  front
 *   inline dtor, deferred, dtor declared last in the class    D1, D0  front
 *   inline dtor, `#pragma defer_codegen off`                  D1, D0  END
 *   out-of-line dtor, deferred codegen                        D0, D1  front
 *
 * Only the out-of-line form gives the cartridge's order (with a homeless D2
 * ahead of it that deadstrips cleanly), and THE CARTRIDGE REFUTES IT. Moving
 * ~daObjUkiyuka_c out of line makes every descendant call it instead of
 * inlining its vptr store, and ov045/daObjKm2_Ukishima_c -- already landed and
 * byte-verified -- drops from 5/5 to 3/5:
 *
 *   DIFF _ZN19daObjKm2_Ukishima_cD1Ev 0x02111b14 size 0x050 -- 999 word(s) differ
 *   DIFF _ZN19daObjKm2_Ukishima_cD0Ev 0x02111b64 size 0x064 -- 999 word(s) differ;
 *        1 reloc destination(s) WRONG (first: data_020a0eac != 0x020112c8:arm9)
 *
 * `999 word(s) differ` is the size disagreement, so that is the ROM saying the
 * base destructor's body is visible to its descendants. The inline form in the
 * header is load-bearing and stays.
 *
 * Under `#pragma defer_codegen off` the pair follows the REVERSE of its first
 * odr-use within one function's codegen, so a single function containing
 * `p->~daObjUkiyuka_c(); delete p;` does emit D0 then D1 -- but the pair lands
 * after that function's own section, and the cartridge puts the pair FIRST in
 * the range. Nothing that emits no code will do it: a `static inline` helper is
 * never codegen'd and so never instantiates the variants at all (measured). So
 * every route to the cartridge's order costs a source-written .text section
 * ahead of the pair, which `compiler_only_output` is not allowed to launder --
 * it is an allow-list for COMPILER-generated output.
 *
 * deslop
 * Leftover: the left TU boundary is tu_map `medium`. If the original TU in fact
 *   began at 0x020b6244 (the three-function run ahead of it, currently
 *   unattributed), the pair would sit MID-range and an odr-use in a preceding
 *   member would produce the cartridge's order with no extra section. That is a
 *   slicing question, not a codegen one, and it is the first thing to test
 *   before trying further source forms here.
 * Leftover: func_ov002_020b6424 is the linker name of this class's shared
 *   Cleanup helper -- ov045 calls it, and its Init counterpart
 *   func_ov002_020b6584 sits immediately above this range in a run of its own.
 *   Naming both belongs in ov002 and is not attempted in this packaging step.
 * Leftover: the class's _ZTV/_ZTI/_ZTS group and the four inherited
 *   vague-linkage RTTI copies are emitted here as key-function passengers. They
 *   are unlicensed until the manifest carries a compiler_only_output block with
 *   a romdata_check verdict per symbol, the way ov002/daBar_c does.
 */

#include "daObjUkiyuka_c.h"

/* Reconciled union of the five legacy files' declarations, each kept at its
   first occurrence. dBgActor_c::IsClsnInRange is NOT declared in
   include/dBgActor_c.h: its ROM name carries by-value Fix12<int> parameters,
   which mwccarm passes differently from the `int` this call site needs, so it
   stays an extern "C" of the mangled symbol, exactly as every other caller in
   the tree spells it. */
extern "C" {
extern int _ZN4dBgW9IsEnabledEv(void *);
extern void _ZN4dBgW7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern s16 data_02082214[];
u16 DecIfAbove0_Short(u16 *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14daObjUkiyuka_c8BehaviorEv, 0x020b6494, size 0xf0      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjUkiyuka_c8BehaviorEv
/* Vtable slot 6, this class's own override -- attributed by the vtable, not by
 * a `recovered name:` comment. daObjUkiyuka_c's vtable (ov002 0x0210912c)
 * differs from dBgActor_c's in slots 6, 9, 16 and 17; this address is slot 6.
 * Being the first out-of-line virtual, it is also this TU's KEY FUNCTION, which
 * is what makes the class's vtable and typeinfo emit here.
 *
 * THE FIELD SPELLINGS ARE THE PRE-MIGRATION ONES, deliberately. mBobPhase is
 * declared s16 but is stepped as a u16 and then sign-read as an s16 in the same
 * breath; mRestTimer is passed to DecIfAbove0_Short by address. Writing either
 * through the member rather than through the cast changes what mwccarm CSEs --
 * see notes/mwccarm-codegen.md on compound assignment -- so the two that matter
 * keep the `(int)this + off` launder and the rest read as members. */
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
/* ROM ordinal 3 -- _ZN14daObjUkiyuka_c6RenderEv, 0x020b646c, size 0x28        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjUkiyuka_c6RenderEv
/* Vtable slot 9. Behavior above already holds the key function, so declaring
 * Render after it is layout- and key-function-neutral. Render dispatches
 * through dBgActor_c's own mModel (0xd4), as every sibling in this series
 * does. */
s32 daObjUkiyuka_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020b6424, 0x020b6424, size 0x48                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6424
/* The shared Cleanup helper: disables the collision mesh if it is live and
 * releases the model and collision SharedFilePtrs the leaf overlay handed in.
 * It keeps C linkage and its address-derived name because that is the name the
 * cartridge's linker carries -- ov045 calls it under this spelling. The
 * pre-migration parameter view (`char *`, `void **`) is kept: it is the only
 * one recovered, and the raw offset is what reproduces the field address. */
extern "C" {
int func_ov002_020b6424(char *t, void **f){
  if(_ZN4dBgW9IsEnabledEv(t+0x124))
    _ZN4dBgW7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(f[0]);
  _ZN13SharedFilePtr7ReleaseEv(f[1]);
  return 1;
}
}
