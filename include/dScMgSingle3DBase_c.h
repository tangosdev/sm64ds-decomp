/* class dScMgSingle3DBase_c, recovered from ROM RTTI + vtable slot identity.
 * Offsets/widths are observed. Names are placeholders.
 *
 * dScMgSingle3DBase_c : dScMgBase_c, confirmed by build/rtti.json
 * (dScMgSingle3DBase_c's __si_class_type_info points at dScMgBase_c,
 * offset 0). It is itself a hierarchy root: 13 direct RTTI children (the
 * "single 3D minigame" family -- card, cup, memory x2, mahjong-carlo x2,
 * roulette, slot3, sound, BSC, snowball, flower, plus dScMg3DEsp_c), all
 * still unnamed. Its own fields therefore start at file-relative offset 0
 * == ROM offset 0x4660 (sizeof(dScMgBase_c)).
 *
 * OWN VTABLE SLOTS, dumped via tools/rtti_vtables.py --own
 * dScMgSingle3DBase_c: 2, 5, 7, 10 re-override slots dScMgBase_c already
 * gave a body (AfterInitResources, AfterCleanupResources, BeforeBehavior,
 * BeforeRender); 16/17 are this class's own D1/D0; 26 and 33 are new
 * overrides (OnHitByCannonBlastedChar-shaped and a VRAM/graphics-bank
 * setup routine respectively). The eight source files still carry an
 * auto-generated "recovered name: dScMgFlower_c_*" comment -- that is the
 * same off-by-one "recovered from vtable slot identity" mislabeling
 * documented for dScMgBase_c's own siblings (an arbitrary concrete
 * descendant's name borrowed for what is really the BASE's own method);
 * the vtable dump above is the authority, not the comment.
 *
 * mSysTracker AT 0x471c IS HAND-VERIFIED, four independent witnesses
 * agreeing on the same offset: this class's own D1 and D0 both destroy it
 * (`_ZN8Particle10SysTrackerD1Ev((char*)c+0x471c)`), AfterInitResources
 * initialises it (`...10InitialiseEv(c+0x471c)`), BeforeBehavior updates it
 * conditionally (`...6UpdateEv(c+0x471c)`). Particle::SysTracker is
 * declared locally rather than shared -- see include/Stage.h's own note on
 * why (two independent gen_header.py shadows, union gives 0x81c, no file
 * here includes either shadow header). This is now a THIRD local copy of
 * the identical type; consolidating all three is a separate change with
 * its own blast radius, not this slice's problem to fix.
 *
 * 0x4700..0x4718 (seven fields) IS NOW SPLIT OUT of the former
 * pad_4660[0xbc]: dScMgRoulette_c's own Render (src/func_ov006_02109834.c)
 * and dScMg3DEsp_c's own Render (src/func_ov006_020e9d1c.cpp) both write
 * these exact offsets, so they're this class's own fields, not either
 * leaf's. Their comments deliberately do NOT use this tree's usual
 * comment-open-then-immediate-hex-literal style: tools/check_header_offsets.py's
 * DATA_SIZE precompute walks a
 * struct's own commented fields by regex to find where a DERIVED class's
 * fields start, and that regex can't parse the namespaced
 * `Particle::SysTracker mSysTracker` a few lines down -- it silently
 * stops at the last field it CAN parse. Before these seven fields existed,
 * nothing in this struct matched that regex at all, so every dependent
 * derived header correctly fell back to this class's asserted `sizeof`.
 * Giving these seven fields the usual hex-comment style made the regex
 * succeed partway through and stop there, undercounting every derived
 * class's own field offsets by nearly the whole SysTracker member --
 * measured directly on dScMgRoulette_c.h, 22/22 fields "mismatched" by
 * exactly that delta before this comment style was changed.
 *
 * 0x4718..0x471b (2 bytes, pad_471a) still HAS NO MATCHED ACCESS -- left
 * as padding.
 *
 * THE DESTRUCTOR IS NOW DEFINED INLINE (fixed after this file's first
 * landing, #1421) -- same fix, same reason, as include/Scene.h's own note:
 * this class has 13 direct RTTI children and every one needs to inline
 * this destructor's body to reproduce its own D1. See the class body's own
 * note for the measurement that caught it. No separate operator delete
 * copy is needed -- dScMgBase_c, this class's IMMEDIATE base, already
 * provides one (see its own header note), and mwcc's inline-D0 route only
 * needs to reach the immediate base. */
#ifndef DSCMGSINGLE3DBASE_C_H
#define DSCMGSINGLE3DBASE_C_H
#include "dScMgBase_c.h"

namespace Particle {
struct SysTracker {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    u8  unk_008;            /* 0x008 */
    u8  pad_009[0x747];
    s32 unk_750;            /* 0x750 */
    u8  mParticle1;          /* 0x754 */
    u8  pad_755[0x7];
    s32 unk_75c;            /* 0x75c */
    u8  mParticle2;          /* 0x760 */
    u8  pad_761[0x7];
    s32 unk_768;            /* 0x768 */
    u8  unk_76c;            /* 0x76c */
    u8  pad_76d[0x7];
    s32 unk_774;            /* 0x774 */
    u8  unk_778;            /* 0x778 */
    u8  pad_779[0x7];
    s32 unk_780;            /* 0x780 */
    u8  unk_784;            /* 0x784 */
    u8  pad_785[0x7];
    s32 unk_78c;            /* 0x78c */
    u8  unk_790;            /* 0x790 */
    u8  pad_791[0x7];
    s32 unk_798;            /* 0x798 */
    u8  mParticle3;          /* 0x79c */
    u8  pad_79d[0x7];
    s32 unk_7a4;            /* 0x7a4 */
    u8  mParticle4;          /* 0x7a8 */
    u8  pad_7a9[0x7];
    s32 unk_7b0;            /* 0x7b0 */
    u8  unk_7b4;            /* 0x7b4 */
    u8  pad_7b5[0xb];
    s32 unk_7c0;            /* 0x7c0 */
    u8  unk_7c4;            /* 0x7c4 */
    u8  pad_7c5[0xf];
    u8  unk_7d4;            /* 0x7d4 */
    u8  pad_7d5[0xf];
    u8  unk_7e4;            /* 0x7e4 */
    u8  pad_7e5[0xb];
    u8  unk_7f0;            /* 0x7f0 */
    u8  pad_7f1[0x3];
    u8  unk_7f4;            /* 0x7f4 */
    u8  pad_7f5[0x3];
    u8  unk_7f8;            /* 0x7f8 */
    u8  pad_7f9[0x3];
    s32 unk_7fc;            /* 0x7fc */
    u8  unk_800;            /* 0x800 */
    u8  pad_801[0x3];
    s32 unk_804;            /* 0x804 */
    u8  unk_808;            /* 0x808 */
    u8  pad_809[0x7];
    u8  unk_810;            /* 0x810 */
    u8  pad_811[0x3];
    u8  unk_814;            /* 0x814 */
    u8  pad_815[0x3];
    u8  unk_818;            /* 0x818 */
    u8  pad_819[0x3];       /* rounds 0x819 up to the 0x81c alignment boundary */

    ~SysTracker();
};

typedef char SysTracker_size_must_be_0x81c[sizeof(SysTracker) == 0x81c ? 1 : -1];
}

struct dScMgSingle3DBase_c : dScMgBase_c {
    /* Declared first, deliberately -- see dScMgBase_c.h's own KEY FUNCTION
       note for why. Overrides slots 16 (D1) and 17 (D0).

       DEFINED INLINE -- same fix, same reason, as include/Scene.h's own
       note: this class has 13 direct RTTI children, and every one of them
       inlines THIS destructor's vptr store + mSysTracker destruction +
       chain to ~dScMgBase_c(), the same way Stage inlines Scene's. Measured
       directly on dScMgMemory_c (one of the 8 in this slice): a merely
       declared `virtual ~dScMgSingle3DBase_c();` compiles a derived
       destructor that references `_ZN19dScMgSingle3DBase_cD2Ev` as an
       undefined external -- no such symbol exists anywhere in the ROM.
       The RAW (pre-migration) recovered destructors confirm why: e.g.
       func_ov006_020f3834 (dScMgMemory_c's own D1, before this slice)
       destroys `_ZN8Particle10SysTrackerD1Ev(c+0x471c)` -- THIS class's
       own member -- directly inline, then calls `_ZN11dScMgBase_cD2Ev(c)`
       directly, with no call to any dScMgSingle3DBase_c-specific
       destructor at all. The out-of-line definition landed in #1421
       because that slice never tested a real descendant; it is a latent
       bug this slice fixes, not a design change. */
    virtual ~dScMgSingle3DBase_c() {}

    /* --- re-overrides of dScMgBase_c's own virtuals, same signature,
           in _ZTV order. Slots 26 and 33 are new at this class; both are
           already matched source (func_ov006_0210a600, func_ov006_0210a708)
           but their signatures are not reconstructed yet, so they stay
           undeclared here rather than guessed. --- */
    virtual void AfterInitResources(u32 vfSuccess); /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                  /* slot  7 */
    virtual int  BeforeRender();                    /* slot 10 */

    /* unk_4700..unk_4718 (offset 0x4700): real matched access, see the
     * file banner's own note on why these comments don't use the usual
     * hex-offset style. */
    u8  pad_4660[0xa0];
    s32 unk_4700; /* offset 0x4700 */
    s32 unk_4704; /* offset 0x4704 */
    s32 unk_4708; /* offset 0x4708 */
    s32 unk_470c; /* offset 0x470c */
    s32 unk_4710; /* offset 0x4710 */
    s32 unk_4714; /* offset 0x4714 */
    s16 unk_4718; /* offset 0x4718 */
    u8  pad_471a[0x2];
    Particle::SysTracker mSysTracker; /* 0x471c */
};

/* NOT a claim that the object ends here -- dScMgSingle3DBase_c has 13 RTTI
   descendants, none of which have touched anything past mSysTracker yet.
   0x471c + sizeof(SysTracker) = 0x4f38 is the minimum claim that unblocks a
   derived class -- if it is short, the first descendant's own fields would
   land on the wrong bytes and build_pin would catch it immediately. */
typedef char dScMgSingle3DBase_c_size_must_be_0x4f38[sizeof(dScMgSingle3DBase_c) == 0x4f38 ? 1 : -1];

#endif
