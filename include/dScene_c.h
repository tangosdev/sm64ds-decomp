#ifndef DSCENE_C_H
#define DSCENE_C_H

#include "dBase_c.h"

/* The scene root: fBase_c -> dBase_c -> dScene_c. Its own code lives at
 * 0x0202e140..0x0202ec9c. (An earlier revision also claimed the four
 * GraphCallback defaults at 0x02018ea0..0x02018ec0; those are
 * dGraph_c::callback_c's virtual slot defaults, not dScene_c's -- see
 * dGraph_c.h -- and dScene_c's own vtable holds none of them.)
 *
 * TEN classes derive from it directly, not the two an earlier draft of this
 * comment named. From the ROM's type graph (tools/rtti_extract.py), every record
 * whose single base is dScene_c, with its vtable:
 *
 *     dScBoot_c   0x02091528     dScGameOver_c 0x020b179c
 *     dScStage_c  0x020921c0     dScMgBase_c   0x020bc0c0
 *     dScMB_c     0x020943c4     dScMiniGm_c   0x020c2490
 *     dScTitle_c  0x020b1650     dScDSMT_c     0x021032e8
 *     dScStarSel_c 0x020b1704    dScEntry_c    0x0211d304
 *
 * Four already have headers under the ROM's own names (dScBoot_c.h,
 * dScMgBase_c.h, dScMiniGm_c.h, dScEntry_c.h). One more has one under a coinage,
 * dScStage_c as include/Stage.h -- and dScBoot_c is also known by the coinage
 * BootScene, from its destructor symbols. The remaining FIVE -- dScMB_c,
 * dScTitle_c, dScStarSel_c, dScGameOver_c, dScDSMT_c -- the tree does not
 * describe at all. 4 + 1 + 5 = 10.
 *
 * Naming a subset is fine. Naming a subset as though it were the set is what
 * this paragraph exists to stop.
 *
 * The header this replaces described dScene_c as a flat 0x14-byte struct with one
 * field. That was a shadow of fBase_c's first twenty bytes, not a class: it
 * named no base, so `dScene_c` and `fBase_c` were unrelated types, and the two
 * translation units that included it could only reach `this` as an int*.
 *
 * EVERY CLAIM BELOW IS READ OUT OF THE ROM.
 *
 * THE CHAIN. Four readings, and the last one is the only one that is decisive
 * on its own. Counted, because a census quoted from memory is how this header
 * got its numbers wrong the first time:
 *
 *   1. _ZTV8dScene_c (0x02092680) is 18 slots. EIGHT still point at fBase_c
 *      implementations -- slots 0, 3, 6, 9, 12, 13, 14, 15 -- and the other TEN
 *      are dScene_c's own: 1, 2, 4, 5, 7, 8, 10, 11, and the destructor pair at
 *      16/17. (An earlier revision of this comment had those two numbers the
 *      wrong way round.)
 *   2. Slot 2 points at dScene_c's own AfterInitResources, whose body is a tail
 *      call to _ZN7dBase_c18AfterInitResourcesEj -- dBase_c's only
 *      FUNCTIONAL override, though it also carries its own D1/D0 at 16/17.
 *      This one SUGGESTS the base and does not force it: an extern "C" call
 *      reproduces the same three words, which is exactly what the .c file this
 *      slice replaced did, and it byte-matched. It constrains what the original
 *      source plausibly was, jointly with the others -- not by itself.
 *   3. dScene_c::~dScene_c says it from the other side: it writes _ZTV8dScene_c, then
 *      _ZTV7dBase_c (0x0208e4b8), and only then calls fBase_c::~fBase_c.
 *      That is the vptr sequence of a three-deep chain with the middle destructor
 *      inlined.
 *   4. The ROM carries its own type graph, and tools/rtti_extract.py reads it.
 *      dScene_c's __si_class_type_info record is at 0x020914d4 under the ROM's real
 *      name for the class, `dScene_c`; its vtable field is 0x02092680, which is
 *      _ZTV8dScene_c, and its single base pointer walks to `dBase_c` (vtable
 *      0x0208e4b8 = _ZTV7dBase_c), whose own base is `fBase_c` (vtable
 *      0x02099edc = _ZTV7fBase_c). `si` means single inheritance: exactly one
 *      base, no ambiguity to resolve.
 *
 * Nintendo EAD's own names for the three, then, are fBase_c -> dBase_c ->
 * dScene_c. The tree's fBase_c/dBase_c/dScene_c are its own coinages, and
 * renaming them is a separate question from getting the shape right.
 *
 * SLOT ORDER is fBase_c's, unchanged -- dScene_c adds no virtual of its own. It
 * overrides eight of them functionally, plus the destructor pair at 16/17, which
 * is where the ten in reading 1 comes from. The destructor is declared FIRST,
 * which is safe for a derived class (an override takes its base's slot
 * wherever it is declared).
 *
 * ~dScene_c() IS NOW DEFINED INLINE, and per include/dBase_c.h that key-
 * function worry is moot -- objisolate makes a key-function TU eligible
 * regardless, by dropping the vtable it emits and rebinding to the ROM's own
 * _ZTV. What forced the inline move instead: Stage::~Stage (and every one of
 * dScene_c's other nine direct children) INLINES dScene_c's own D2 the same way
 * dScene_c inlines dBase_c's -- the ROM's Stage destructor stores Stage's
 * vptr, then dScene_c's, then dBase_c's, then calls fBase_c's D2 directly,
 * with no call to a separate dScene_c::~dScene_c(). A merely declared
 * `virtual ~dScene_c();` can't be inlined -- the compiler has no body to see --
 * and emits `bl _ZN8dScene_cD2Ev` where the ROM has none (measured on a Stage
 * trial: 80 bytes with the call vs the ROM's 104 with none). dScene_c had been
 * defined out-of-line since the previous slice; that blocked every child's
 * own destructor migration the same way an out-of-line ~dBase_c() would
 * have blocked dScene_c's.
 *
 * The cost is what dBase_c already pays: src/_ZN8dScene_cD1Ev.cpp and
 * _ZN8dScene_cD0Ev.cpp can no longer DEFINE ~dScene_c() (that would be a
 * redefinition of the inline body below) and a bare include emits nothing --
 * `_ZN8dScene_cD1Ev is not in the object`. Both now carry a forcing call
 * instead; see the note in each file.
 *
 * LAYOUT. dScene_c declares no fields, and that is a claim, so here is its basis.
 *
 * First, no dScene_c method reads or writes anything past fBase_c's own members.
 * The only this-relative access in the entire class, across all 23 functions, is
 * `this->pauseFlags` in BeforeBehavior at 0x13; the destructors touch the vptr at
 * 0x0; everything else either passes `this` straight through or never sees it.
 *
 * Second -- and this is the half that actually rules out a hidden field -- Stage
 * putting its first own field at 0x050 is NOT on its own enough. It is consistent
 * with a dScene_c field at 0x50 that only Stage's code happens to touch. What
 * separates the two is that Stage is not the only child: dScBoot_c's generated
 * header puts a **u16** at 0x050 where Stage has a u8. Two siblings disagreeing
 * about the type at 0x050 means 0x050 is not inherited, so it belongs to each of
 * them and not to dScene_c.
 *
 * The size assertion at the bottom holds all three headers to that: it fails to
 * compile if anyone adds a member anywhere in the chain.
 *
 * STATIC vs NON-STATIC cannot be decided from a definition -- both spellings
 * mangle identically, and a method that ignores `this` compiles the same either
 * way. It is decided at the CALL SITE, and the argument takes two different
 * shapes depending on whether the function has parameters at all:
 *
 *   - For the ones that DO take arguments -- SetFaders, SetSceneToSpawn,
 *     StartSceneFade -- the call site puts the first declared argument in r0,
 *     leaving no register for a `this`. SetAndStopColorFader loads a FaderColor
 *     into r0 and `bl`s SetFaders; Stage::Behavior calls SetSceneToSpawn with
 *     `mov r0,#4; mov r1,#0`.
 *   - For the ones that take NONE -- SetAndStopColorFader, SpawnIfNecessary,
 *     PrepareToSpawnBoot, Initialise3dGraphics, ResetHardwareRegisters -- that
 *     argument is unavailable, because nothing is placed in r0 at all. The
 *     evidence is the mirror image: r0 still holds whatever the previous call
 *     left in it, and a member call would have had to overwrite it with `this`.
 *     Initialise3dGraphics reaches ResetHardwareRegisters with r0 carrying an
 *     unrelated return value.
 *   - ResetFadersAndSound is the one that is NOT static: BeforeInitResources
 *     (itself a virtual, so r0 is a real `this`) calls it with a plain `bl` and
 *     r0 untouched, and the body consumes r0 -- it stores it to 0x0209f5c0 and
 *     passes it on. Its mangled name takes no parameter, so a static could not
 *     receive that register.
 *
 * This header has no C spelling and cannot be included from a C file, because
 * dBase_c.h has none either. Nothing includes it from C: the two .c files
 * that used to are part of this slice and are now .cpp.
 */
struct FaderBrightness;

/* The actor heap and its deallocator, for the inline operator delete below.
   Spelt exactly as include/decl_common.h spells it -- see the note in dActor_c.h. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct dScene_c : dBase_c {
    /* Declared first, deliberately -- see KEY FUNCTION above. Overrides slots
       16 (D1) and 17 (D0); the position in this list does not affect that.
       DEFINED INLINE on purpose: subclass destructors inline it, the same
       way dBase_c's is inline for dScene_c's own sake. See the long note
       above. */
    virtual ~dScene_c() {}

    /* dScene_c's own copy of dActor_c's inline operator delete. mwcc inlines the
       operator only when it finds it in the class itself or its IMMEDIATE
       base, and dScene_c's immediate base is dBase_c, which has none --
       fBase_c's copy (declared on dActor_c, not fBase_c) is out of reach
       from this branch of the hierarchy entirely. This copy is also what
       every one of dScene_c's ten direct subclasses (Stage, BootScene, and the
       rest -- see the census above) will find as ITS immediate base, so it
       unlocks their D0s too. See the long comment in dActor_c.h for why an
       inline member is what the ROM shows. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- overrides, in _ZTV8dScene_c order. Signatures must match fBase_c's
           declarations exactly or these become new slots instead of overrides. --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual int  BeforeCleanupResources();             /* slot  4 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual void AfterBehavior(u32 vfSuccess);         /* slot  8 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void AfterRender(u32 vfSuccess);           /* slot 11 */

    /* --- non-virtual, and takes `this`: BeforeInitResources `bl`s here
           with r0 untouched. --- */
    int ResetFadersAndSound();

    /* --- static: every call site in the ROM puts the first declared argument in
           r0, so none of these receives a `this`. --- */
    static void SetFaders(FaderBrightness *fader);
    static void SetAndStopColorFader();
    static void StartSceneFade(u32 sceneID, u32 param, u16 fadeColor);
    static int  SetSceneToSpawn(u32 sceneID, u32 param);
    static int  SpawnIfNecessary();
    static void PrepareToSpawnBoot();
    static void Initialise3dGraphics();
    static void ResetHardwareRegisters();

};

/* Holds fBase_c, dBase_c and dScene_c to the layout the paragraph above
   claims. A silently-added member anywhere in the chain fails this. */
typedef char dScene_c_size_must_be_0x50[sizeof(dScene_c) == 0x50 ? 1 : -1];

#endif
