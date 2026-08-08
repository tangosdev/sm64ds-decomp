#ifndef SCENE_H
#define SCENE_H

#include "ActorDerived.h"

/* The scene root: ActorBase -> ActorDerived -> Scene, and Scene is itself the base
 * of Stage and BootScene. Its own code lives at 0x0202e140..0x0202ec9c, plus the
 * four GraphCallbacks stranded at 0x02018ea0..0x02018ec0.
 *
 * The header this replaces described Scene as a flat 0x14-byte struct with one
 * field. That was a shadow of ActorBase's first twenty bytes, not a class: it
 * named no base, so `Scene` and `ActorBase` were unrelated types, and the two
 * translation units that included it could only reach `this` as an int*.
 *
 * EVERY CLAIM BELOW IS READ OUT OF THE ROM.
 *
 * THE CHAIN. Four independent readings, and the last one is decisive.
 *
 *   1. _ZTV5Scene (0x02092680) is 18 slots, and ten of them still point at
 *      ActorBase implementations.
 *   2. Slot 2 points at Scene's own AfterInitResources, which is a tail call to
 *      _ZN12ActorDerived18AfterInitResourcesEj -- and slot 2 is the one and only
 *      slot ActorDerived overrides, so Scene could not be naming that function
 *      unless ActorDerived were a base.
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
 * SLOT ORDER is ActorBase's, unchanged -- Scene adds no virtual of its own, it
 * only overrides eight. The destructor is therefore declared FIRST here, which is
 * safe for a derived class (an override takes its base's slot wherever it is
 * declared) and is deliberate: it makes ~Scene the key function, and ~Scene is
 * only ever defined as an extern "C" free function in _ZN5SceneD0Ev.c and
 * _ZN5SceneD1Ev.c. No translation unit defines the key function, so CW 1.2 emits
 * no vtable group, and the copy the module's gap object supplies from ROM data
 * stands alone. This is the same arrangement include/ActorDerived.h documents.
 *
 * LAYOUT. Scene declares no fields, and that is a claim, so here is its basis: no
 * Scene method reads or writes anything past ActorBase's own members -- the only
 * field access in the whole class is `this->unk_013` in BeforeBehavior, at 0x13 --
 * and Stage, which derives from Scene, puts its first own field at 0x050, exactly
 * sizeof(ActorBase). The size assertion at the bottom is what holds the three
 * headers to it: it fails to compile if anyone adds a member anywhere in the
 * chain.
 *
 * STATIC vs NON-STATIC cannot be decided from a definition -- both spellings
 * mangle identically, and a method that ignores `this` compiles the same either
 * way. It is decided at the CALL SITE, and every one of them is in the ROM:
 * Scene::SetAndStopColorFader loads a FaderColor into r0 and branches straight to
 * SetFaders, so r0 is SetFaders' first declared parameter and not a `this`.
 * BeforeInitResources, by contrast, branches to ResetFadersAndSound with r0
 * untouched, which is a `this` passthrough. The two groups below are split on that
 * evidence, function by function.
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

    /* --- non-virtual, and takes `this`: BeforeInitResources tail-branches here
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
