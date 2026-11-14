/* The base of the level-object family: platforms, lifts, blocks and the rest of
 * the things a level is built out of. A model, a moving mesh collider, and the
 * matrix that keeps the collider where the model is.
 *
 * Layout evidence and the sizeof-0x320 argument: notes/platform-provenance.md. */
#ifndef DBGACTOR_C_H
#define DBGACTOR_C_H
#include "types.h"

/* fwd */
struct daPly_c;
struct player_;

/* MEASURED -- MUST STAY FIRST, ahead of dExtSimpleModel_c.h. Matrix4x3 has two guarded
   spellings sharing the same 0x30 bytes; whichever a TU sees first stands, and
   dExtSimpleModel_c.h pulls in the wrong one. UpdateClsnPosAndRot then compiles to 0x74
   against the ROM's 0x64. notes/platform-provenance.md. */
#include "common.h"

#include "dActor_c.h"
#include "dExtSimpleModel_c.h"
#include "dBgW_KcMbg.h"

struct dBgActor_c : dActor_c {
    u8  pad_0d0[0x4];
    dExtSimpleModel_c mModel;       /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;       /* 0x124 */
    /* Where the collider sits: mModel's matrix with the actor's position
       written over the translation row. See UpdateClsnPosAndRot. */
    Matrix4x3 mClsnMat;     /* 0x2ec */
    /* Mega Mario's knockback: KillByMegaChar arms the flag and loads the
       30-frame timer; UpdateKillByMegaChar flies the actor while the flag is
       set and calls Kill() when the timer runs out. */
    u8 mMegaKnockback;      /* 0x31c */
    u8 mMegaKnockbackTimer; /* 0x31d */
    /* The class ends at 0x31e; sizeof rounds to 0x320, and derived classes do
       use that tail padding (daObjRc_Guruguru_c::mAngVelY). */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* MEASURED -- INLINE ON PURPOSE, do not move out of line. Every subclass
       destructor inlines this body; an out-of-line declaration makes each one
       emit a `bl` the ROM does not have, and gives this class a key function.
       notes/platform-provenance.md. */
    virtual ~dBgActor_c() {}

    /* Slot 31, the only new virtual this class adds -- and the key function, so
       the TU defining it also emits _ZTV10dBgActor_c and _ZTI10dBgActor_c.
       MEASURED against the cartridge's 33-word table; without it every subclass
       vtable comes out a word short. notes/platform-provenance.md. */
    virtual void Kill();

    /* --- non-virtual --- */
    /* DECLARED, defined out of line in src/runtime/actor/d_bg_actor.cpp as real
     * C++. Base-only class: the ROM function is the dActor_c base step, the
     * vptr store, and the two member constructions in declaration order. */
    dBgActor_c();

    void KillByMegaChar(daPly_c &player_);
    void func_ov002_020ee5d0(int);
    void UpdateClsnPosAndRot();
    void UpdateModelPosAndRotY();

    /* The two IsClsnInRange* symbols used to spell a by-value Fix12<int>, and
       this header already recorded that "their bytes refuse it". Acted on
       2026-10-23: both are real methods taking the scalar fx32, under
       _ZN10dBgActor_c13IsClsnInRangeEii and
       _ZN10dBgActor_c21IsClsnInRangeOnScreenEii, reproducing ov002
       0x020ee870/0xc4 and 0x020ee934/0x11c. notes/mwccarm-codegen.md 6az.
       Parameters (radius, yOffset) are unnamed on purpose: a named
       class-scope parameter consumes an @NNN number in every consumer. */
    int IsClsnInRange(fx32, fx32);
    int IsClsnInRangeOnScreen(fx32, fx32);

    /* Takes the scalar fx32, under _ZN10dBgActor_c20UpdateKillByMegaCharEsssi.
       The body at ov002 0x020ee674 only forwards the pivot height in a
       register, so the scalar reproduces it (2026-10-26), and callers can
       call it as a method. notes/mwccarm-codegen.md 6az. */
    int UpdateKillByMegaChar(s16 a, s16 b, s16 c, fx32 d);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgActor_c_size_must_be_0x320[sizeof(dBgActor_c) == 0x320 ? 1 : -1];
#endif



#endif
