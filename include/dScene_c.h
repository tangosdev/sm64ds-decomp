#ifndef DSCENE_C_H
#define DSCENE_C_H

#include "dBase_c.h"

/* The scene root: fBase_c -> dBase_c -> dScene_c. Ten classes derive from it
 * directly; Stage (dScStage_c), BootScene (dScBoot_c) and dScEntry_c are the
 * three this tree describes. It declares no data members of its own -- every
 * offset below 0x50 belongs to fBase_c.
 *
 * Derivation, the vtable slot census, the layout argument and the call-site
 * evidence for which methods are static: notes/scene-provenance.md and
 * notes/dscene-c-siblings-census.md.
 *
 * This header has no C spelling and cannot be included from a C file, because
 * dBase_c.h has none either.
 */
struct FaderBrightness;

/* The actor heap and its deallocator, for the inline operator delete below.
   Spelt exactly as include/decl_common.h spells it -- see the note in dActor_c.h. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct dScene_c : dBase_c {
    /* No separate dScene_c constructor exists in the ROM. This body is
       inlined into scene subclasses between dScene_c's vptr store and the
       subclass vptr store. */
    dScene_c();

    /* Declared first (key function) and DEFINED INLINE, both deliberately.
       MEASURED: every subclass destructor inlines this one, so a merely
       declared `virtual ~dScene_c();' emits `bl _ZN8dScene_cD2Ev' where the
       ROM has none and costs 24 bytes in Stage's destructor alone. Do not move
       it out of line. Overrides slots 16 (D1) and 17 (D0). */
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~dScene_c() {}   /* no slot */
#else
    virtual ~dScene_c() {}   /* D1 and D0 */
#endif

    /* dScene_c's own copy of dActor_c's inline operator delete. mwcc inlines
       the operator only when it finds it in the class itself or its IMMEDIATE
       base, and dScene_c's immediate base is dBase_c, which has none. This
       copy is also what all ten direct subclasses find as THEIR immediate
       base, so it unlocks their D0s too. */
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
    /* `bool`, not `int`. The body returns only 0 or 1 (`mov r0,#1` /
       `moveq r0,#0`), so both spellings emit the same ARM bytes here and at
       every call site, byte-verified. The reason to prefer `bool` is
       Stage::BeforeInitResources, whose entire body is a tail call to this one:
       vtable slot 1 is declared `bool`, and while the two types disagreed the
       only ROM-faithful spelling over there was a bare call with no `return`,
       because `return ResetFadersAndSound();` made the compiler insert an
       int->bool normalisation the ROM does not have. With both sides `bool` the
       `return` is free. That matters off the cartridge: a host compiler does not
       promise to pass r0 through a returnless function, so slot 1 was handing
       the host port garbage. See src/_ZN5Stage19BeforeInitResourcesEv.cpp. */
    bool ResetFadersAndSound();

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

inline dScene_c::dScene_c()
{
    pauseFlags |= 1;
    pauseFlags |= 4;
}

/* Holds fBase_c, dBase_c and dScene_c to the layout the paragraph above
   claims. A silently-added member anywhere in the chain fails this. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScene_c_size_must_be_0x50[sizeof(dScene_c) == 0x50 ? 1 : -1];
#endif

#endif
