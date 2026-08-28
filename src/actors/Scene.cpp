//cpp
/* Translation unit, reconstructed and promoted -- arm9 / dScene_c (the ROM's own name for the class is
 * dScene_c; see include/dScene_c.h reading 4).
 *
 * CANONICAL. The ROM build compiles this file and takes all 21 functions from
 * this one object; the one-function sources that owned 0x0202e140..0x0202ec9c
 * are gone. See notes/translation-unit-reconstruction-plan.md (the design) and
 * notes/tu-reconstruction-pilot-report.md (the worked example this file was
 * hand-assembled against, because `tubuild.py create` has no candidate to
 * offer here -- see BOUNDARY below).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order. The reading order below is
 * therefore ResetHardwareRegisters first and ~dScene_c last. The rule holds
 * between distinct definitions but NOT inside the destructor's variant group,
 * and not for the vague-linkage out-of-line copy of an inline function, which
 * trails the whole file -- both apply here; see the manifest's
 * `verification.criteria.functions_occur_in_expected_order`.
 *
 * ============================================================================
 * BOUNDARY -- WHY THESE TWENTY-ONE FUNCTIONS AND NOT SOME OTHER SET
 * ============================================================================
 *
 * tools/tu_map.py CANNOT SEGMENT THIS MODULE. It emits `main` as a single unit
 * covering 0x20049f0..0x20707a4 with 2984 functions and 70-odd class labels, so
 * there is no `tubuild.py create` candidate id for dScene_c and no tu_map boundary
 * to lean on. Everything below was established by hand from the cartridge.
 *
 * 1. THE RUN IS CONTIGUOUS AND ITS EDGES ARE FOREIGN. 0x0202e140..0x0202ec9c is
 *    a gapless sequence of twenty-one functions in config/arm9/delinks.txt.
 *    Immediately below it is func_0202e118 (0x0202e118..0x0202e140), and below
 *    that _ZN5StageC3Ev; immediately above it is func_0202ec9c. Neither edge
 *    function is a dScene_c member and neither is reached from one.
 *
 * 2. TWO FUNCTIONS INSIDE THE SPAN CARRY OTHER CLASSES' NAMES, AND THEY BELONG
 *    HERE ANYWAY. Stage::SetVramBanks (0x0202e73c) and GX::DisableAllBanks
 *    (0x0202e78c) sit between dScene_c::Initialise3dGraphics (0x0202e6c8) and
 *    dScene_c::ResetHardwareRegisters (0x0202e7d4). They are not stapled on: an
 *    object's contribution to a section is one contiguous run, so if the two
 *    dScene_c functions that straddle them share an object then so do they. What
 *    proves the straddle is the DATA side --
 *
 * 3. STATIC-DATA ATTRIBUTION PINS BOTH SIDES OF THE SPAN TO ONE OBJECT. This
 *    TU's statics are one unbroken .data block, immediately followed by the
 *    vtable it is the key-function TU for:
 *
 *        0x02092660  data_02092660   u8    "pending scene already spawned"
 *        0x02092664  data_02092664   u16   pending scene ID (0x187 = none)
 *        0x02092668  data_02092668   Matrix2x2, 0x10 bytes -> 0x02092678
 *        0x02092678  _ZTV8dScene_c's Itanium preamble (offset-to-top, typeinfo)
 *        0x02092680  _ZTV8dScene_c, 18 slots -> 0x020926c8
 *        0x020926c8  data_020926c8   the next object's contribution
 *
 *    From config/arm9/relocs.txt, every reference to those three statics comes
 *    from inside this span and nowhere else -- and they come from BOTH SIDES of
 *    the two foreign-named functions:
 *
 *        data_02092660 <- 0x0202e2f0, 0x0202e344, 0x0202e5e8   (below the pair)
 *        data_02092664 <- 0x0202e2f4, 0x0202e340, 0x0202e390,
 *                         0x0202e5c4                            (below the pair)
 *        data_02092668 <- 0x0202ebe4                             (ABOVE the pair)
 *
 *    0x0202ebe4 is inside dScene_c::ResetHardwareRegisters. Objects contribute to
 *    every section in one ascending, disjoint interval each, so a single object
 *    owns 0x02092660..0x020926c8; the functions below the pair and the function
 *    above it both reach into that one object's data; therefore they are one
 *    object; therefore the pair between them is in it too.
 *
 * 4. THE SOURCE ORDER THE REVERSAL IMPLIES IS A SENSIBLE FILE. Reversing the ROM
 *    address order yields BeforeInitResources/AfterInitResources, then the
 *    Cleanup pair, then Behavior, then Render -- each Before immediately before
 *    its After, the four groups in the order fBase_c declares them, and the
 *    destructor last. That is corroboration, not proof, but a wrong cut does not
 *    usually reverse into a tidy file.
 *
 * THE FOUR GraphCallbacks ARE NOT IN THIS TU. _ZN8dGraph_c10callback_c14GraphCallback0Ev..3Ev
 * live at 0x02018ea0..0x02018ec0, 0x15,2a0 bytes (86 KB) BELOW this run. A
 * single object's .text contribution is one contiguous interval, so they cannot
 * share an object with 0x0202e140..0x0202ec9c -- this is the same rule item 3
 * leans on, used in the other direction, and it is decisive on its own. Three
 * further readings agree: their own neighbours are func_02018e68 (an
 * OS_SleepThread wrapper) and func_02018ec0 (a fader slot-2 dispatch), i.e. a
 * system/graphics grab-bag rather than a scene class; the pointer tables that
 * hold them (0x0208ee14..0x0208ee20) sit ~0x3800 bytes below _ZTV8dScene_c's .data
 * block, on the far side of _ZTV7dBase_c, so they fall in a much earlier
 * object's data contribution; and their own address order reverses to
 * GraphCallback0,1,2,3, a natural block, which says they are consecutive
 * definitions in SOME file -- just not this one. A class's members may of course
 * be spread over several .cpp files, and dScene_c's are.
 *
 * ============================================================================
 * HOW FAR UP THE LADDER THIS GOT, AND WHAT STOPPED IT
 * ============================================================================
 *
 * text-verified: 21/21 MATCH, objisolate clean, reloc-destinations clean,
 * emission order ROM-ascending. Baseline control first -- the 21 untouched
 * legacy files re-verified 21/21 under the pinned 2004/b56 with the strict
 * relocation-destination check on -- so a green merged result is not an
 * environment artefact.
 *
 * partial-link-verified (`tubuild.py linkcheck --partial`): one compile of this
 * file, objisolate.derive per function, all 21 derived objects substituted at
 * the existing per-function object paths with delinks.txt UNCHANGED. 21/21
 * contribution-equivalent; the range 0x0202e140..0x0202ec9c comes out
 * IDENTICAL (2908 bytes, 0 differing); module fidelity 106/106 exact at
 * 100.000000%; and the full 16 MB ROM builds BIT-IDENTICAL to the baseline
 * control that substitutes nothing.
 *
 * WHOLE-RANGE link-verified is WALLED, and the wall is not in this source.
 * Defining dScene_c's key function emits _ZTV8dScene_c STB_GLOBAL, which dsd's gap
 * object already supplies from ROM data, and mwldarm aborts with
 * `Multiply-defined: "virtual table for dScene_c" in dScene_c.o / Previously defined
 * in _dsd_gap@main_44.o`. Two separate blockers, and the second is the one that
 * generalises:
 *
 *   A. TOOLING, fixable in principle. The TU would have to OWN the .data its
 *      vtable lands in so the gap object stops claiming it, and
 *      tubuild.splice_tu_entry hardcodes a single `.text` line with no path for
 *      any other section. The range is known (BOUNDARY item 3); writing it is
 *      not currently possible. The vtable's own 20 words are plausible content
 *      -- every slot points at a real ROM function -- so this is plumbing.
 *
 *   B. NAMING, AND NOT FIXABLE BY ATTRIBUTION AT ALL. The object also emits six
 *      STB_LOPROC RTTI records the audit calls HOMELESS: _ZTI/_ZTS for dScene_c,
 *      dBase_c, fBase_c, named in no symbols.txt anywhere. The reason is
 *      that a _ZTS record's BYTES ARE THE CLASS NAME STRING, and this tree's
 *      names are coinages while the cartridge holds Nintendo's:
 *
 *          ROM  0x020914b0 : '8dScene_c\0'   (then '9dScBoot_c\0')
 *          ours _ZTS8dScene_c : '8dScene_c\0'      -- 7 bytes, not 10, and different
 *
 *      There is no address in the ROM those three records could be attributed
 *      to, because their content is not in the ROM. rombuild links -nodead so
 *      nothing drops them either. THE GENERAL RULE: no key-function TU can
 *      reach whole-range link-verified while its class name is a coinage. The
 *      only fix is renaming to the ROM's own names (dScene_c -> dScene_c,
 *      dBase_c -> dBase_c, fBase_c -> fBase_c, per include/dScene_c.h
 *      reading 4), which is a separate tree-wide change.
 *
 * objisolate drops exactly that material per function, which is why --partial
 * reproduces the module and the whole-range splice cannot. See the manifest
 * entry for the measured details.
 *
 * THIS TU ALSO OWNS, and this round neither declares nor verifies any of it:
 *   data_02092660  .data  0x02092660   (u8)
 *   data_02092664  .data  0x02092664   (u16)
 *   data_02092668  .data  0x02092668   (Matrix2x2, the identity affine)
 *   _ZTV8dScene_c     .data  0x02092680   (slot array; the C++ object starts
 *                                       8 bytes earlier, at 0x02092678)
 * Compiling dScene_c's key function here does emit a vtable and RTTI records as a
 * side effect. That is expected and is inventoried by `tubuild.py compile`, not
 * licensed.
 */
#include "Stage.h"        /* -> dScene_c.h -> dBase_c.h -> fBase_c.h, and the
                             Stage class SetVramBanks below is a member of */
#include "FaderColor.h"   /* -> FaderBrightness.h -> Fader.h */

/* ------------------------------------------------------------------------- */
/* ROM ordinal 20 -- dScene_c::ResetHardwareRegisters, 0x0202e7d4, size 0x4c8    */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static: Initialise3dGraphics calls it with a bare `bl` and no argument setup.
 */

/* Completes the type include/G2x.h forward-declares. The legacy file declared
   it here too; it is left at this, its first occurrence, rather than hoisted. */
struct Matrix2x2 { int m[4]; };

/* GX is a namespace, and DisableAllBanks is DEFINED BY THIS TU, two functions
   down. The legacy callers each declared it as `extern "C" void
   _ZN2GX15DisableAllBanksEv(void)` -- the final linker spelling written out by
   hand -- which was correct while the definition lived in another object, but in
   a merged TU it would put an extern "C" declaration and a real namespace-scope
   definition on the SAME linker symbol. Declared as what it is instead; the
   emitted `bl` is identical, and this is now checked rather than asserted. */
namespace GX { void DisableAllBanks(); }

extern "C" {
extern int  func_02053c10(int enable);
extern int  func_02053be0(int enable);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(volatile void* p, Matrix2x2* m, int a, int b, int c, int d);
extern void func_02053a90(u16* out, int v);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern Matrix2x2 data_02092668;
}

void dScene_c::ResetHardwareRegisters()
{
    Matrix2x2 m;

    m = data_02092668;

    *(volatile u16*)0x4000304 |= 0x8000;
    *(volatile u16*)0x4000304 = (*(volatile u16*)0x4000304 & 0xfffffdf1) | 0x20e;
    *(volatile u16*)0x4000304 |= 1;

    GX::DisableAllBanks();
    func_02053c10(0);
    func_02053be0(1);
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);

    *(volatile u32*)0x4000000 &= ~0x1f00;
    *(volatile u32*)0x4000000 &= ~0xe000;
    *(volatile u32*)0x4000000 &= ~0x38000000;
    *(volatile u32*)0x4000000 &= ~0x7000000;
    *(volatile u32*)0x4000000 &= 0xffcfffef;
    *(volatile u32*)0x4000000 &= 0xffbfff9f;
    *(volatile u32*)0x4000000 &= ~0x800000;
    *(volatile u32*)0x4000064 = 0x80000000;

    *(volatile u16*)0x4000008 &= 0x43;
    *(volatile u16*)0x400000a &= 0x43;
    *(volatile u16*)0x400000c &= 0x43;
    *(volatile u16*)0x400000e &= 0x43;
    *(volatile u16*)0x4000008 &= ~0x40;
    *(volatile u16*)0x400000a &= ~0x40;
    *(volatile u16*)0x400000c &= ~0x40;
    *(volatile u16*)0x400000e &= ~0x40;
    *(volatile u16*)0x4000008 &= ~3;
    *(volatile u16*)0x400000a &= ~3;
    *(volatile u16*)0x400000c &= ~3;
    *(volatile u16*)0x400000e &= ~3;

    *(volatile u32*)0x4000010 = 0;
    *(volatile u32*)0x4000014 = 0;
    *(volatile u32*)0x4000018 = 0;
    *(volatile u32*)0x400001c = 0;

    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4000020, &m, 0, 0, 0, 0);
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4000030, &m, 0, 0, 0, 0);

    *(volatile u16*)0x4000048 &= ~0x3f;
    *(volatile u16*)0x4000048 &= ~0x3f00;
    *(volatile u16*)0x400004a &= ~0x3f;
    *(volatile u16*)0x400004a &= ~0x3f00;

    *(volatile u16*)0x4000040 = 0;
    *(volatile u16*)0x4000044 = 0;
    *(volatile u16*)0x4000042 = 0;
    *(volatile u16*)0x4000046 = 0;
    *(volatile u8*)0x400004c = 0;
    *(volatile u8*)0x400004d = 0;

    func_02053a90((u16*)0x400006c, 0);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32*)0x4001000 &= ~0x1f00;
    *(volatile u32*)0x4001000 &= ~0xe000;
    *(volatile u32*)0x4001000 &= 0xffcfffef;
    *(volatile u32*)0x4001000 &= 0xffbfff9f;
    *(volatile u32*)0x4001000 &= ~0x800000;

    *(volatile u16*)0x4001008 &= 0x43;
    *(volatile u16*)0x400100a &= 0x43;
    *(volatile u16*)0x400100c &= 0x43;
    *(volatile u16*)0x400100e &= 0x43;
    *(volatile u16*)0x4001008 &= ~0x40;
    *(volatile u16*)0x400100a &= ~0x40;
    *(volatile u16*)0x400100c &= ~0x40;
    *(volatile u16*)0x400100e &= ~0x40;
    *(volatile u16*)0x4001008 &= ~3;
    *(volatile u16*)0x400100a &= ~3;
    *(volatile u16*)0x400100c &= ~3;
    *(volatile u16*)0x400100e &= ~3;

    *(volatile u32*)0x4001010 = 0;
    *(volatile u32*)0x4001014 = 0;
    *(volatile u32*)0x4001018 = 0;
    *(volatile u32*)0x400101c = 0;

    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4001020, &m, 0, 0, 0, 0);
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4001030, &m, 0, 0, 0, 0);

    *(volatile u16*)0x4001048 &= ~0x3f;
    *(volatile u16*)0x4001048 &= ~0x3f00;
    *(volatile u16*)0x400104a &= ~0x3f;
    *(volatile u16*)0x400104a &= ~0x3f00;

    *(volatile u16*)0x4001040 = 0;
    *(volatile u16*)0x4001044 = 0;
    *(volatile u16*)0x4001042 = 0;
    *(volatile u16*)0x4001046 = 0;
    *(volatile u8*)0x400104c = 0;
    *(volatile u8*)0x400104d = 0;

    func_02053a90((u16*)0x400106c, 0);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 19 -- GX::DisableAllBanks, 0x0202e78c, size 0x48              */
/* ------------------------------------------------------------------------- */
/* recovered: language-mode migration only. GX is a namespace, not a class: no
 * `this`, no vtable, no layout.
 *
 * It has six callers ROM-wide (0x02005a64, 0x0200f2d4, 0x0202cf1c, 0x0202e744,
 * 0x0202e820, 0x0203053c), two of them inside this file -- so it is a genuine
 * external-linkage definition that happens to live in the scene TU, not an
 * inline body the compiler dragged in.
 */
extern "C" {
extern void func_02053f58(void);
extern void func_02054018(void);
extern void func_02054004(void);
extern void func_02053f6c(void);
extern void func_02053fa8(void);
extern void func_02053f94(void);
extern void func_02053f80(void);
extern void func_02053fe0(void);
extern void func_02053fbc(void);
extern void func_02053f44(void);
extern void func_02053f30(void);
extern void func_02053f08(void);
extern void func_02053ee0(void);
}

namespace GX {
void DisableAllBanks(){
 func_02053f58(); func_02054018(); func_02054004(); func_02053f6c();
 func_02053fa8(); func_02053f94(); func_02053f80(); func_02053fe0();
 func_02053fbc(); func_02053f44(); func_02053f30(); func_02053f08();
 func_02053ee0();
}
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 18 -- Stage::SetVramBanks, 0x0202e73c, size 0x50              */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static: sets the VRAM bank assignment for a level and never touches an object.
 * Its one caller is 0x0202cf20, inside Stage::InitResources -- a different TU.
 * A Stage member physically resident in the dScene_c TU is unusual but is what the
 * data attribution in the header comment establishes; see BOUNDARY item 3.
 */
extern "C" {
extern void _ZN2GX13SetBankForTexEt(u16);
extern void _ZN2GX17SetBankForTexPlttEt(u16);
extern void _ZN2GX12SetBankForBGEt(u16);
extern void _ZN2GX13SetBankForOBJEt(u16);
extern void _ZN2GX15SetBankForSubBGEt(u16);
extern void _ZN2GX22SetBankForSubBGExtPlttEt(u16);
extern void _ZN2GX16SetBankForSubOBJEt(u16);
}

void Stage::SetVramBanks() {
    GX::DisableAllBanks();
    _ZN2GX13SetBankForTexEt(3);
    _ZN2GX17SetBankForTexPlttEt(0x30);
    _ZN2GX12SetBankForBGEt(8);
    _ZN2GX13SetBankForOBJEt(0x40);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX22SetBankForSubBGExtPlttEt(0x80);
    _ZN2GX16SetBankForSubOBJEt(0x100);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 17 -- dScene_c::Initialise3dGraphics, 0x0202e6c8, size 0x74      */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static: BeforeInitResources reaches it with a bare `bl`, and it in turn calls
 * ResetHardwareRegisters the same way. Wipes the hardware back to a known state,
 * puts the 3D engine in mode 1, then sets the clear colour, clear depth and
 * viewport before handing off to the shared initialiser at 0x0205583c.
 */
extern "C" {
extern void func_0205583c(void);
/* The free function of the same name, at namespace scope. The `::` at the call
   site is what keeps it apart from the member being defined here. */
extern void Initialise3dGraphics(int arg);
}

void dScene_c::Initialise3dGraphics()
{
    ResetHardwareRegisters();
    func_0205583c();
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    *(volatile u32 *)0x40004c8 = 0x296a5800;
    *(volatile u32 *)0x40004cc = 0x7fff;
    *(volatile u32 *)0x40004c0 = 0x7fff;
    *(volatile u32 *)0x40004c4 = 0;
    ::Initialise3dGraphics(0);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 16 -- dScene_c::ResetFadersAndSound, 0x0202e66c, size 0x5c       */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ member, shared header.
 *
 * Non-virtual, and takes `this`: it publishes the scene as the current actor at
 * 0x0209f5c0 and hands the same pointer to fBase_c::BeforeInitResources.
 *
 * The qualified call is deliberate. fBase_c::BeforeInitResources is virtual and
 * dScene_c overrides it, so an unqualified call here would dispatch through the
 * vtable and land straight back in the caller -- infinite recursion. The ROM does
 * `bl 0x02043c78`, a direct call to the base implementation.
 */

/* `extern` on every one of these -- a braced `extern "C" { }` is a linkage
   specification wrapped around a DEFINITION, not a declaration. */
extern "C" {
extern fBase_c *data_0209f5c0;
/* RECONCILED. Three of this TU's legacy files declared 0x0209f5e8 three
   different ways -- `FaderBrightness` (SetAndStopColorFader, which uses
   ::speed, and ResetFadersAndSound, which takes its address), `FaderColor`
   (StartSceneFade, which uses ::color) and a local one-field shadow
   (BeforeBehavior, which dispatches through its vptr with the wrong arity;
   see there). FaderColor is the most complete of the three and is what the
   object really is -- __sinit_02074edc constructs it up the chain and leaves
   the FaderColor vtable, data_0208eb2c, in its vptr -- so it is declared once,
   here at its first use, as that. ResetFadersAndSound's note that
   include/FaderColor.h "does not name a base" was true when it was written and
   is now stale: FaderColor.h's C++ half is `struct FaderColor : FaderBrightness`
   since the fader-hierarchy fix, so the upcast SetFaders needs is implicit,
   single-inheritance and offset-free. */
extern FaderColor data_0209f5e8;
/* `void*`, not ResetFadersAndSound's `u32`: BeforeBehavior both compares it
   against 0 and stores &data_0209f5d0 into it, which is the more complete of
   the two observations. Assigning 0 costs the same store either way. */
extern void *data_0209f1e4;
extern void func_02011b7c(void);
}

int dScene_c::ResetFadersAndSound()
{
    data_0209f5c0 = this;
    if (!fBase_c::BeforeInitResources())
        return 0;
    SetFaders(&data_0209f5e8);
    data_0209f1e4 = 0;
    func_02011b7c();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 15 -- dScene_c::BeforeInitResources, 0x0202e638, size 0x34       */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 1.
 *
 * `ResetFadersAndSound()` is called on `this`: the ROM branches to 0x0202e66c
 * with r0 untouched, which is a `this` passthrough and is why that one is not
 * static.
 */
bool dScene_c::BeforeInitResources()
{
    if (!ResetFadersAndSound())
        return false;
    Initialise3dGraphics();
    return true;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 14 -- dScene_c::AfterInitResources, 0x0202e62c, size 0xc         */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 2.
 *
 * A tail call to dBase_c::AfterInitResources (0x02013ef4), not to
 * fBase_c's. Slot 2 is dBase_c's only FUNCTIONAL override, so a dScene_c
 * forwarding here is a strong hint that dBase_c is a base -- a hint, not a
 * proof; the RTTI chain and the destructor's vptr sequence are what settle it.
 * See readings 3 and 4 in include/dScene_c.h.
 */
void dScene_c::AfterInitResources(u32 vfSuccess)
{
    dBase_c::AfterInitResources(vfSuccess);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 13 -- dScene_c::BeforeCleanupResources, 0x0202e5f0, size 0x3c    */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 4.
 *
 * Chains to fBase_c's, gives up if it fails, and otherwise tears down the
 * object at 0x0209b53c through 0x02011974. The early exit is a real early
 * return, not a predicated fall-through: the ROM ends the failing path with its
 * own `addeq sp,#4; ldmeq sp!,{lr}; bxeq lr` rather than branching to the tail.
 */
extern "C" {
extern void *data_0209b53c;
extern void func_02011974(void *object);
}

int dScene_c::BeforeCleanupResources()
{
    if (!fBase_c::BeforeCleanupResources())
        return 0;
    func_02011974(&data_0209b53c);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 12 -- dScene_c::AfterCleanupResources, 0x0202e5d0, size 0x20     */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 5.
 *
 * This is the function that proves how the `vfSuccess` argument arrives: the
 * first instruction is `cmp r1,#2`, so the parameter is in r1 and r0 is the
 * `this` the method never otherwise touches. Every other Before/After pair in
 * the class inherits its argument convention from this one reading.
 *
 * It ends in the same `ldr ip,[pc]; bx ip` tail call as the three-word veneers,
 * in a function that plainly is not one -- which is the evidence that the shape
 * belongs to -interworking tail calls in general and says nothing about branch
 * range.
 */
extern "C" {
extern u8 data_02092660;   /* set once the pending scene has been spawned */
}

void dScene_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2)
        data_02092660 = 0;
    fBase_c::AfterCleanupResources(vfSuccess);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 11 -- dScene_c::BeforeBehavior, 0x0202e3d4, size 0x1fc           */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 7.
 *
 * Chains to fBase_c's, then runs the scene-transition state machine: hold the
 * screen while a fade is in flight, and once the brightness fader reaches the
 * end, queue the next scene and mark this one for destruction.
 *
 * ONE DELIBERATE SHADOW REMAINS, AND IT IS EVIDENCE, NOT LAZINESS. The current
 * fader at 0x0209f5bc is reached through the file-local `FaderVTable` below
 * rather than through include/FaderBrightness.h, because the ROM disagrees with
 * the fader's own mangled names about how many arguments slots 3 and 4 take:
 *
 *     0202e4d8  ldr r0,[pc,#0xec]      ; &current fader
 *     0202e4dc  mov r1,#0x1e
 *     0202e4e4  mov r2,#0              <-- a SECOND argument
 *     0202e4ec  ldr r3,[r3,#0x10]      ; slot 4
 *     0202e4f0  blx r3
 *
 * and the same shape at 0x0202e58c for slot 3. But the functions those slots
 * hold are _ZN15FaderBrightness14SetForwardTimeEj and ...15SetBackwardTimeEj,
 * and `Ej` is one parameter. Calling them through the real class would drop the
 * `mov r2,#0` and stop matching, so the ROM's own dScene_c translation unit must
 * have been built against a two-argument prototype. The names below are the
 * ROM's -- read out of data_0208eb2c -- and only the arity is this file's.
 *
 * WHAT CONSOLIDATION CHANGED HERE, and nothing else: the legacy file declared
 * data_0209f5e8 and data_0209f5bc as its own one-field `FaderObject`. Both are
 * now declared once, at their first use, as the real classes (FaderColor and
 * FaderBrightness *), and the two sites that need the wrong-arity dispatch cast
 * to FaderObject * at the point of use. The cast is address-preserving --
 * FaderObject's single member is the vptr the C++ classes place at offset 0 --
 * so the emitted loads are the same two the legacy file emitted.
 */

/* The ROM's fader vtable at data_0208eb2c, with the arity the call sites above
   prove this TU was compiled against. Slots 0/1 are D1/D0. The slot names come
   from what the table's own entries resolve to -- the tree has no _ZTV symbol
   for this address, only the data_ placeholder. */
struct FaderVTable {
    void (*D1)(void *);
    void (*D0)(void *);
    void (*AdvanceFade)(void *);
    void (*SetBackwardTime)(void *, u32, u32);
    void (*SetForwardTime)(void *, u32, u32);
    int  (*IsAtStart)(void *);
    int  (*IsAtEnd)(void *);
};
struct FaderObject { FaderVTable *vt; };

extern "C" {
extern u8   data_0209f1e0;
/* `FaderBrightness *`, not the legacy `FaderObject *`: SetFaders below
   dispatches slots 8 and 9 on it through the real class, which is the more
   complete observation of the two. */
extern FaderBrightness *data_0209f5bc;   /* the currently installed fader */
extern FaderBrightness data_0209f5d0;
extern u16  data_02092664;               /* pending scene ID; 0x187 means none */

extern void func_02023544(void);
extern void _ZN15FaderBrightness14SetForwardTimeEj(FaderBrightness *self, u32 frames);
extern int  _ZN15FaderBrightness7IsAtEndEv(FaderBrightness *self);
extern int  func_020431c4(fBase_c *self);
}

int dScene_c::BeforeBehavior()
{
    if (!fBase_c::BeforeBehavior())
        return 0;

    if (data_0209f1e0 != 0) {
        int noActor = (data_0209f5c0->actorID == 0);
        if (noActor != 0) {
            func_02023544();
        } else {
            if (data_0209f1e4 == 0) {
                data_0209f5d0.currInterp = 0;   /* 0x4, not speed at 0x8 */
                _ZN15FaderBrightness14SetForwardTimeEj(&data_0209f5d0, 0x10);
                data_0209f1e4 = &data_0209f5d0;
            } else if (_ZN15FaderBrightness7IsAtEndEv(&data_0209f5d0)) {
                StartSceneFade(1, 0, 0);
                ((FaderObject *)&data_0209f5e8)->vt->SetForwardTime((FaderObject *)&data_0209f5e8, 0, 0);
                MarkForDestruction();
            }
            return 0;
        }
    }

    if (data_02092664 != 0x187) {
        if (((FaderObject *)data_0209f5bc)->vt->IsAtStart(data_0209f5bc) != 0) {
            ((FaderObject *)data_0209f5bc)->vt->SetForwardTime(data_0209f5bc, 0x1e, 0);
        } else if (((FaderObject *)data_0209f5bc)->vt->IsAtEnd(data_0209f5bc) != 0) {
            MarkForDestruction();
        }
        return 1;
    }

    if ((pauseFlags & 1) != 0) {
        if (func_020431c4(this) == 0) {
            pauseFlags &= ~1;
            pauseFlags &= ~4;
        }
        return 0;
    } else {
        if (((FaderObject *)data_0209f5bc)->vt->IsAtEnd(data_0209f5bc) != 0) {
            ((FaderObject *)data_0209f5bc)->vt->SetBackwardTime(data_0209f5bc, 0x1e, 0);
        }
        return 1;
    }
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 10 -- dScene_c::AfterBehavior, 0x0202e3c8, size 0xc              */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 8.
 *
 * A tail call to fBase_c::AfterBehavior (0x02043af8); see AfterRender below
 * for why that is three words and why the parameter type comes from the
 * target's declaration rather than from these bytes.
 */
void dScene_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 9 -- dScene_c::BeforeRender, 0x0202e3a4, size 0x24               */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 10.
 *
 * Chains to fBase_c's and narrows the result to 0 or 1. The narrowing is in
 * the ROM, not invented here: after the call the bytes are `cmp r0,#0;
 * movne r0,#1; moveq r0,#0`, which is exactly what a `!= 0` costs and would be
 * absent if the result were returned unchanged.
 *
 * Return type is `int` because fBase_c declares slot 10 as `int`; an override
 * whose return type differs is not an override at all, it is a nineteenth slot.
 */
int dScene_c::BeforeRender()
{
    return fBase_c::BeforeRender() != 0;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- dScene_c::AfterRender, 0x0202e398, size 0xc                 */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ virtual override -- vtable slot 11.
 *
 * The whole body is a tail call to fBase_c::AfterRender (0x02043ac4), which
 * mwccarm emits under -interworking as `ldr ip,[pc]; bx ip; .word target`
 * rather than a plain `b`. The three-word shape is about interworking, not
 * distance: the target here is 87KB away, comfortably inside `b` range.
 *
 * The parameter type is NOT derived from these bytes -- it cannot be, because a
 * tail call never touches r0-r3, so any prototype at all would reproduce them.
 * It comes from the definition of the target, include/fBase_c.h slot 11.
 */
void dScene_c::AfterRender(u32 vfSuccess)
{
    fBase_c::AfterRender(vfSuccess);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- dScene_c::SetSceneToSpawn, 0x0202e36c, size 0x2c            */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static: the mangled name says two u32 parameters, and the ROM reads them from
 * r0 and r1. A non-static member would put them in r1 and r2 behind a `this`.
 *
 * Records the scene to switch to and reports whether it changed anything -- the
 * caller only pays for the fade if the answer is yes.
 */
extern "C" {
extern u32 data_0209f5b8;   /* the pending scene's parameter word */
}

int dScene_c::SetSceneToSpawn(u32 sceneID, u32 param)
{
    if (sceneID != data_02092664) {
        data_02092664 = sceneID;
        data_0209f5b8 = param;
        return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- dScene_c::StartSceneFade, 0x0202e348, size 0x24             */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static; r0/r1/r2 carry the three declared parameters. Queues the scene change
 * and, only if that changed anything, sets the colour the fader will fade
 * through.
 *
 * The `t` in the mangled name is `unsigned short`, and it is load-bearing: the
 * store is `strh`, so the field at FaderColor+0xc is two bytes wide.
 */
void dScene_c::StartSceneFade(u32 sceneID, u32 param, u16 fadeColor)
{
    if (SetSceneToSpawn(sceneID, param))
        data_0209f5e8.color = fadeColor;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- dScene_c::PrepareToSpawnBoot, 0x0202e300, size 0x48         */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static. Selects which scene boots next -- 0 when 0x0203d9b4 says so,
 * otherwise 0x168 -- and clears the "already spawned" flag so SpawnIfNecessary
 * will act.
 */
extern "C" {
extern int func_0203d9b4(void);
}

void dScene_c::PrepareToSpawnBoot()
{
    if (func_0203d9b4())
        data_02092664 = 0;
    else
        data_02092664 = 0x168;
    data_02092660 = 0;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- dScene_c::SpawnIfNecessary, 0x0202e26c, size 0x94           */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static. Spawns the pending scene actor unless one has already been spawned
 * (data_02092660) or the pending ID is the 0x187 sentinel meaning "none". On
 * success it writes the sentinel back and raises the flag, so the work happens
 * once.
 */
extern "C" {
extern int func_02013edc(u32 sceneID, u32 param, int a);
}

int dScene_c::SpawnIfNecessary()
{
    u16 sceneID;
    if (data_02092660 != 0 || (sceneID = data_02092664) == 0x187)
        return 0;
    {
        int spawned = func_02013edc(sceneID, data_0209f5b8, 1);
        if (spawned == 0)
            return 0;
        data_02092664 = 0x187;
        data_02092660 = 1;
        return spawned;
    }
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- dScene_c::SetFaders, 0x0202e1e0, size 0x8c                  */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Installs a fader, carrying the outgoing one's end state across so the screen
 * does not jump: if the fader being replaced was sitting at the start, snap the
 * new one to the start; if it was at the end, snap it to the end; if it was
 * mid-fade, leave the new one alone.
 *
 * This is a STATIC member -- r0 is the parameter, not a `this` -- and the
 * argument is a FaderBrightness, which the mangled name has said all along and
 * which dScene_c::SetAndStopColorFader confirms by loading a FaderColor into r0
 * and branching straight here. The IsAtStart/IsAtEnd/SetToStart/SetToEnd calls
 * are vtable slots 5/6/9/8; include/Fader.h declared seven slots and the ROM
 * has ten, which is why this function could not be migrated with the rest of
 * the class until #1259.
 */
extern "C" {
/* Two globals holding the same pointer. 0x0209f5bc is the one this function
   reads back; 0x0209d4ac is read by 0x02018efc, which calls AdvanceFade (slot 2)
   on it -- which is the evidence that it holds a fader and not a dScene_c. */
extern FaderBrightness *data_0209d4ac;
}

void dScene_c::SetFaders(FaderBrightness *fader)
{
    if (data_0209f5bc) {
        if (data_0209f5bc->IsAtStart()) {
            fader->SetToStart();
        } else if (data_0209f5bc->IsAtEnd()) {
            fader->SetToEnd();
        }
    }
    data_0209f5bc = fader;
    data_0209d4ac = fader;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- dScene_c::SetAndStopColorFader, 0x0202e1b4, size 0x2c       */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ static member, shared header.
 *
 * Static, and this function is the evidence that SetFaders is static too: it
 * loads &data_0209f5e8 into r0 and `bl`s SetFaders without writing r1. If
 * SetFaders took a `this`, r0 would be it, and this call would be handing a
 * fader to a dScene_c*.
 *
 * The `speed = 0` afterwards stops the fade where SetFaders left it. Offset 0x8
 * is Fader::speed, pinned by Fader::AdvanceInterp picking its target from that
 * word's sign.
 */
void dScene_c::SetAndStopColorFader()
{
    SetFaders(&data_0209f5e8);
    data_0209f5e8.speed = 0;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN8dScene_cD1Ev  0x0202e140  size 0x30   (complete-object destructor)      */
/*   _ZN8dScene_cD0Ev  0x0202e170  size 0x44   (deleting destructor)             */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ destructor, DEFINED INLINE IN include/dScene_c.h.
 *
 * The body is not here and must not be moved here. ~dScene_c() is defined in the
 * class body because Stage::~Stage and every one of dScene_c's other nine direct
 * children INLINE it -- the ROM's Stage destructor stores Stage's vptr, then
 * dScene_c's, then dBase_c's, then calls fBase_c's D2 directly, with no
 * call to a separate dScene_c::~dScene_c(). A compiler can only do that from a
 * visible body. The same relationship one level up is what gives dScene_c's own
 * destructor its two vptr stores:
 *
 *     str r2, [r4]        ; _ZTV8dScene_c
 *     str r1, [r4]        ; _ZTV7dBase_c   <- dBase_c's D2, INLINED
 *     bl  fBase_c::~fBase_c
 *
 * That second store exists only because ~dBase_c() is defined inline in
 * include/dBase_c.h. Outlining it would emit `bl _ZN7dBase_cD2Ev`
 * where the ROM has a store, and dScene_c would stop matching -- which is the
 * first place to look if the destructor ever comes out one store short. Note
 * that a byte MATCH alone cannot see this, since both vptr stores are relocated
 * words and therefore wildcards to match.compare; the two-store SHAPE, and
 * reloc_audit's destination check on them, is the real test.
 *
 * Because the definition is in the header, this TU cannot define it again, and
 * merely including the header emits nothing at all -- `_ZN8dScene_cD1Ev is not in
 * the object`. The two uses below force the out-of-line copies into existence,
 * exactly as src/_ZN8dScene_cD1Ev.cpp and src/_ZN8dScene_cD0Ev.cpp do one apiece.
 * They are never called; they are UNLICENSED extra .text this TU emits, listed
 * as such by `tubuild.py compile`, and they are the reason this file cannot be
 * promoted as-is.
 */
/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN8dScene_cD1Ev  0x0202e140  size 0x30   (complete-object destructor)      */
/*   _ZN8dScene_cD0Ev  0x0202e170  size 0x44   (deleting destructor)             */
/* ------------------------------------------------------------------------- */
/* THERE IS DELIBERATELY NO CODE HERE, AND NO FORCING SCAFFOLD EITHER.
 *
 * ~dScene_c() is defined INLINE in include/dScene_c.h and must stay there: Stage and
 * every one of dScene_c's other nine direct children INLINE it -- the ROM's Stage
 * destructor stores Stage's vptr, then dScene_c's, then dBase_c's, then calls
 * fBase_c's D2 directly, with no call to a separate dScene_c::~dScene_c(). A
 * compiler can only do that from a visible body. The same relationship one level
 * up is what gives dScene_c's own destructor its TWO vptr stores, read here
 * word-by-word off the cartridge at 0x0202e140:
 *
 *     0202e150  e5842000   str r2, [r4]   ; r2 = 0x02092680 = _ZTV8dScene_c
 *     0202e154  e5841000   str r1, [r4]   ; r1 = 0x0208e4b8 = _ZTV7dBase_c
 *     0202e158  eb0056fa   bl  0x02043d48 ; _ZN7fBase_cD2Ev
 *
 * The second store exists ONLY because ~dBase_c() is defined inline in
 * include/dBase_c.h. Outlining it emits `bl _ZN7dBase_cD2Ev` where
 * the ROM has a store -- one store where the ROM has two -- and dScene_c stops
 * matching. If this destructor ever comes out one store short, that header is
 * the first place to look. Note that a byte MATCH alone cannot see this: both
 * vptr stores are relocated words and therefore wildcards to match.compare.
 * What actually checks it is reloc_audit's destination check, which reports
 * both stores landing on 0x02092680 and 0x0208e4b8 exactly as
 * config/arm9/relocs.txt records them.
 *
 * WHY NO SCAFFOLD. src/_ZN8dScene_cD1Ev.cpp and src/_ZN8dScene_cD0Ev.cpp each carry a
 * never-called forcing function (`p->~dScene_c();` / `delete p;`) because a TU that
 * merely includes the header emits no out-of-line copy -- their own comments
 * record `_ZN8dScene_cD1Ev is not in the object`. THAT IS NOT TRUE OF THIS TU, and
 * the difference is measured, not assumed: this file defines dScene_c's key
 * function (BeforeInitResources, the first non-inline virtual dScene_c declares),
 * so it emits _ZTV8dScene_c, whose slots 16 and 17 ARE D1 and D0 -- and taking
 * their addresses for the table is itself what forces both out-of-line copies
 * into existence. Compiled with the scaffold and without it, `tubuild.py
 * compile` reports D1 at section[16] size 0x30 and D0 at section[18] size 0x44
 * either way; dropping it took UNLICENSED function symbols from 1 to 0.
 *
 * And the scaffold is not merely redundant, it is harmful. It is 0x38 bytes of
 * STB_GLOBAL .text that no config/arm9/symbols.txt entry names, sitting INSIDE
 * this TU's licensed span. objisolate can strip such a thing per function, but a
 * merged object cannot be reduced that way and rombuild links -nodead, so it
 * would displace every byte after it in a whole-range link. Measured elsewhere
 * in this workstream on arm9/dActor_c: with a forcing scaffold the TU range came
 * out 506 bytes different and 15/106 modules exact; with the scaffold gone,
 * IDENTICAL and 106/106.
 */
