/* PORT_HOST_ABI. The fader hierarchy, force-included ahead of
 * src/_ZN8dScene_c9SetFadersEP15FaderBrightness.cpp so that translation unit
 * dispatches the ROM's vtable slots instead of MSVC's. Nothing in src/ or
 * include/ changes. Run link100, lane CRASH2.
 *
 * The mechanism is the one port/unmatched/MgSmartball_HostAbi.h already uses:
 * pre-define a header's own guard macro, so the src file still says
 * `#include "FaderBrightness.h"`, the include still happens, and the body is
 * skipped because the guard is already defined.
 *
 * ---- WHAT IS WRONG WITHOUT THIS ----------------------------------------
 *
 * include/Fader.h declares `virtual ~Fader()` first and then eight more
 * virtuals. mwccarm gives a virtual destructor TWO vtable entries (D1 at slot
 * 0, D0 at slot 1), so the ROM's fader tables are ten words long and
 * IsAtStart, IsAtEnd, SetToEnd and SetToStart are slots 5, 6, 8 and 9 --
 * which is what include/FaderBrightness.h's own comment says out loud:
 * "dScene_c::SetFaders dispatches slot 9 at [vt+0x24] and slot 8 at
 * [vt+0x20]". MSVC folds the destructor pair into ONE slot, so the same
 * source compiled for the host puts those four at 4, 5, 7 and 8. Every fader
 * table the port actually seats is ROM-shaped and ten words long
 * (?vt_HalFaderWipe@@6B@ in port/hal/fader_wipes.cpp, and the hosted ROM table
 * data_0208eb2c in port/hal/scene_boot.cpp), so the caller lands one slot low.
 * This is the shift recorded in the memory note sm64ds-port-msvc-dtor-slot-shift.
 *
 * MEASURED, not inferred. In the shipped artifact
 * ?SetFaders@dScene_c@@SAXPAUFaderBrightness@@@Z at 004d4010 calls [eax+0x10]
 * where it means IsAtStart. On a HalFaderWipe that word is
 * ?SetForwardTime@HalFaderWipe@@UAEHHH@Z at 00416190, a two-int __thiscall
 * member ending `ret 8` at 004161ab. Called with nothing pushed it pops eight
 * bytes of SetFaders' own frame: esp is 001af188 at the call and 001af190
 * after it. SetFaders' epilogue then pops the wrong words and returns into
 * st_binit's tail rather than to its caller, so dScene_c::ResetFadersAndSound
 * never reaches its `mov al, 1`; Stage's vtable slot 1 answers 0; the init
 * dispatcher skips slot 0, which is the level boot; port_stage_a_boot returns
 * null and port/tests/walk_window.cpp:8335 dereferences it on frame one.
 *
 * ---- WHY THIS SHAPE ----------------------------------------------------
 *
 * Only the two slots that TAKE ARGUMENTS can corrupt a frame, because only
 * they clean the stack: a caller that lands one slot low on a no-argument slot
 * gets the wrong answer and keeps its frame. So the fatal call sites are the
 * ones aiming at slot 5 (IsAtStart, landing on SetForwardTime) and slot 4
 * (SetForwardTime, landing on SetBackwardTime). SetFaders is the one such site
 * on the boot path, and it is also the only translation unit left that reaches
 * a fader through the real C++ class at all -- src/_ZN8dScene_c14BeforeBehaviorEv.cpp
 * already declares a file-local explicit-slot `FaderVTable` of cdecl function
 * pointers for exactly this reason, and says so in its own header comment.
 *
 * THIS IS A CALL-SITE FIX, NOT THE WHOLE ANSWER. The complete fix is the
 * pattern the memory note proved byte-neutral over 1,142 objects, applied to
 * include/Fader.h itself:
 *
 *     #ifdef _MSC_VER
 *         virtual void Destructor1();
 *         virtual void Destructor0();
 *     #else
 *         virtual ~Fader();
 *     #endif
 *
 * That would fix every consumer at once and is where this belongs. It is a
 * decomp-side edit, which this lane does not own, so it is written up in
 * out/CRASH2/report.md and handed over instead.
 *
 * The class is left ABSTRACT on purpose: the two destructor slots below are
 * declared and never defined, and nothing here is ever instantiated, so no
 * vtable is emitted for either class and no unresolved external appears. This
 * translation unit only ever holds faders as pointers.
 */
#ifndef PORT_FADER_HOSTABI_H
#define PORT_FADER_HOSTABI_H

#include "types.h"

/* shadows include/Fader.h */
#define FADER_H
struct Fader {
    Fix12i currInterp;  /* 0x04 -- current fade level, 0..0x1000 */
    Fix12i speed;       /* 0x08 -- per-frame delta; sign selects the target */

    /* The ROM's destructor PAIR, spelled as two ordinary virtuals so MSVC
       gives them the two slots mwccarm gives them and everything below keeps
       its ROM slot number. */
    virtual void Destructor1();                     /* slot 0 */
    virtual void Destructor0();                     /* slot 1 */

    virtual void AdvanceFade() = 0;                 /* slot 2 */
    virtual int  SetBackwardTime(u32 frames) = 0;   /* slot 3 */
    virtual int  SetForwardTime(u32 frames) = 0;    /* slot 4 */
    virtual int  IsAtStart() = 0;                   /* slot 5 */
    virtual int  IsAtEnd() = 0;                     /* slot 6 */
    virtual int  IsBetweenStartAndEnd() = 0;        /* slot 7 */
    virtual void SetToEnd() = 0;                    /* slot 8 */
    virtual void SetToStart() = 0;                  /* slot 9 */
};

/* shadows include/FaderBrightness.h. It overrides slots 2..9 and adds none, so
   for slot-numbering purposes it is Fader; keeping it empty avoids declaring
   overrides that would need definitions. The NAME and the `struct` keyword are
   what MSVC's mangling encodes, so
   ?SetFaders@dScene_c@@SAXPAUFaderBrightness@@@Z is unchanged. */
#define FADERBRIGHTNESS_H
struct FaderBrightness : Fader {
};

#endif
