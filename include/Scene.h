#ifndef SCENE_H
#define SCENE_H

#include "ActorDerived.h"

/* The scene root: ActorBase -> ActorDerived -> Scene. Its own code lives at
 * 0x0202e140..0x0202ec9c, plus the four GraphCallbacks stranded at
 * 0x02018ea0..0x02018ec0.
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
 * The header this replaces described Scene as a flat 0x14-byte struct with one
 * field. That was a shadow of ActorBase's first twenty bytes, not a class: it
 * named no base, so `Scene` and `ActorBase` were unrelated types, and the two
 * translation units that included it could only reach `this` as an int*.
 *
 * EVERY CLAIM BELOW IS READ OUT OF THE ROM.
 *
 * THE CHAIN. Four readings, and the last one is the only one that is decisive
 * on its own. Counted, because a census quoted from memory is how this header
 * got its numbers wrong the first time:
 *
 *   1. _ZTV5Scene (0x02092680) is 18 slots. EIGHT still point at ActorBase
 *      implementations -- slots 0, 3, 6, 9, 12, 13, 14, 15 -- and the other TEN
 *      are Scene's own: 1, 2, 4, 5, 7, 8, 10, 11, and the destructor pair at
 *      16/17. (An earlier revision of this comment had those two numbers the
 *      wrong way round.)
 *   2. Slot 2 points at Scene's own AfterInitResources, whose body is a tail
 *      call to _ZN12ActorDerived18AfterInitResourcesEj -- ActorDerived's only
 *      FUNCTIONAL override, though it also carries its own D1/D0 at 16/17.
 *      This one SUGGESTS the base and does not force it: an extern "C" call
 *      reproduces the same three words, which is exactly what the .c file this
 *      slice replaced did, and it byte-matched. It constrains what the original
 *      source plausibly was, jointly with the others -- not by itself.
 *   3. Scene::~Scene says it from the other side: it writes _ZTV5Scene, then
 *      _ZTV12ActorDerived (0x0208e4b8), and only then calls ActorBase::~ActorBase.
 *      That is the vptr sequence of a three-deep chain with the middle destructor
 *      inlined.
 *   4. The ROM carries its own type graph, and tools/rtti_extract.py reads it.
 *      Scene's __si_class_type_info record is at 0x020914d4 under the ROM's real
 *      name for the class, `dScene_c`; its vtable field is 0x02092680, which is
 *      _ZTV5Scene, and its single base pointer walks to `dBase_c` (vtable
 *      0x0208e4b8 = _ZTV12ActorDerived), whose own base is `fBase_c` (vtable
 *      0x02099edc = _ZTV9ActorBase). `si` means single inheritance: exactly one
 *      base, no ambiguity to resolve.
 *
 * Nintendo EAD's own names for the three, then, are fBase_c -> dBase_c ->
 * dScene_c. The tree's ActorBase/ActorDerived/Scene are its own coinages, and
 * renaming them is a separate question from getting the shape right.
 *
 * SLOT ORDER is ActorBase's, unchanged -- Scene adds no virtual of its own. It
 * overrides eight of them functionally, plus the destructor pair at 16/17, which
 * is where the ten in reading 1 comes from. The destructor is declared FIRST, which is
 * safe for a derived class (an override takes its base's slot wherever it is
 * declared) and is deliberate: it makes ~Scene the key function, and ~Scene is
 * only ever defined as an extern "C" free function in _ZN5SceneD0Ev.c and
 * _ZN5SceneD1Ev.c. No translation unit defines the key function, so CW 1.2 emits
 * no vtable group, and the copy the module's gap object supplies from ROM data
 * stands alone. This is the same arrangement include/ActorDerived.h documents.
 *
 * LAYOUT. Scene declares no fields, and that is a claim, so here is its basis.
 *
 * First, no Scene method reads or writes anything past ActorBase's own members.
 * The only this-relative access in the entire class, across all 23 functions, is
 * `this->unk_013` in BeforeBehavior at 0x13; the destructors touch the vptr at
 * 0x0; everything else either passes `this` straight through or never sees it.
 *
 * Second -- and this is the half that actually rules out a hidden field -- Stage
 * putting its first own field at 0x050 is NOT on its own enough. It is consistent
 * with a Scene field at 0x50 that only Stage's code happens to touch. What
 * separates the two is that Stage is not the only child: dScBoot_c's generated
 * header puts a **u16** at 0x050 where Stage has a u8. Two siblings disagreeing
 * about the type at 0x050 means 0x050 is not inherited, so it belongs to each of
 * them and not to Scene.
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
 * ActorDerived.h has none either. Nothing includes it from C: the two .c files
 * that used to are part of this slice and are now .cpp.
 */
struct FaderBrightness;

struct Scene : ActorDerived {
    /* Declared first, deliberately -- see KEY FUNCTION above. Overrides slots
       16 (D1) and 17 (D0); the position in this list does not affect that. */
    virtual ~Scene();

    /* --- overrides, in _ZTV5Scene order. Signatures must match ActorBase's
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

    /* Scene-graph traversal hooks, called through a table of plain function
       addresses -- which is itself why they cannot be non-static members. All four
       are the same two instructions, `mov r0,#1; bx lr`. */
    static int GraphCallback0();
    static int GraphCallback1();
    static int GraphCallback2();
    static int GraphCallback3();
};

/* Holds ActorBase, ActorDerived and Scene to the layout the paragraph above
   claims. A silently-added member anywhere in the chain fails this. */
typedef char Scene_size_must_be_0x50[sizeof(Scene) == 0x50 ? 1 : -1];

#endif
