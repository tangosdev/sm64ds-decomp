/* PORT_HOST_ABI. include/Fader.h's class with the ROM's two destructor slots,
 * force-included ahead of the two matched IsBetweenStartAndEnd bodies,
 *
 *     src/_ZN7dWipe_c20IsBetweenStartAndEndEv.cpp
 *     src/engine/fader/_ZN15FaderBrightness20IsBetweenStartAndEndEv.cpp
 *
 * so their two unqualified predicate calls dispatch the slots the cartridge
 * dispatches. Nothing in src/ or include/ changes. Run linkfull, lane PSYCHE1.
 *
 * Same mechanism as unmatched/Fader_HostAbi.h (run link100, lane CRASH2, for
 * dScene_c::SetFaders): pre-define the real header's guard, so the src file's
 * own #include still happens and its body is skipped. This one shadows Fader
 * ALONE. FaderBrightness.h, FaderColor.h and dWipe_c.h are the real headers,
 * so every member these two TUs define or call keeps the mangled name the rest
 * of the link already carries; each derived class's own `virtual ~X()` becomes
 * a new slot after slot 9, which neither TU dispatches.
 *
 * ---- WHAT IS WRONG WITHOUT THIS ----------------------------------------
 *
 * Both ROM bodies (0x0202ed7c and 0x02017628) answer "between" by asking the
 * RECEIVER's own slots 5 and 6: `ldr r1,[r1,#0x14]` / `blx r1`, then
 * `ldr r1,[r1,#0x18]` / `blx r1`. include/Fader.h spells the destructor pair as
 * one `virtual ~Fader()`, which MSVC gives one slot, so the host build of both
 * bodies calls [vt+0x10] and [vt+0x14] instead. On the dWipe_c table
 * (data_020926f0, hal/fdr_arm9_fader_seat.cpp) +0x10 is SetForwardTime. Measured
 * on the INT48 build under cdb: every minigame exit fade polls this predicate,
 * each poll reached fdr_s10 with the caller's saved esi as its "receiver" and
 * was refused, and once that stray word WAS the fader, so SetForwardTime ran
 * with 0x0048b256 as its frame count. The predicate then answered from IsAtStart
 * alone. It is also the one caller that keeps slot 0x10 from serving the
 * two-argument __thiscall callers properly: see fdr_s10 in that file.
 */
#ifndef PORT_FADERSLOTS_HOSTABI_H
#define PORT_FADERSLOTS_HOSTABI_H

#include "types.h"

/* shadows include/Fader.h: the same object, the same members, the ROM's slot
   numbers */
#define FADER_H
extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct Fader {
    Fix12i currInterp;  /* 0x04 */
    Fix12i speed;       /* 0x08 */

    /* The ROM's destructor PAIR as two ordinary virtuals, declared and never
       defined: neither TU constructs, destroys or emits a table for a fader. */
    virtual void Destructor1();                     /* slot 0 */
    virtual void Destructor0();                     /* slot 1 */

    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

    virtual void AdvanceFade() = 0;                 /* slot 2 */
    virtual int  SetBackwardTime(u32 frames) = 0;   /* slot 3 */
    virtual int  SetForwardTime(u32 frames) = 0;    /* slot 4 */
    virtual int  IsAtStart() = 0;                   /* slot 5 */
    virtual int  IsAtEnd() = 0;                     /* slot 6 */
    virtual int  IsBetweenStartAndEnd() = 0;        /* slot 7 */
    virtual void SetToEnd() = 0;                    /* slot 8 */
    virtual void SetToStart() = 0;                  /* slot 9 */

    void AdvanceInterp();
};

#endif
