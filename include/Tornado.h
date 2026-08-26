/* Started life AUTO-GENERATED from matched-function evidence by
 * tools/gen_header.py; the field names below have since been recovered from
 * the bodies. Offsets/widths are observed, not guessed. Gaps are explicit
 * padding. Renaming cannot change codegen.
 *
 * A THREE-STATE MACHINE. Tornado::Behavior switches on mState and calls one
 * of three nonvirtual members, all of which live in this class's recovered
 * original translation unit:
 *   0  Tornado::State0 -- dormant at home. Holds the scale at 0
 *      until a player comes within 0x5dc000, then spins up over 0x3c frames
 *      and hands over to state 1.
 *   1  Tornado::State1 -- hunting. Steers mPrevAngleY toward the
 *      player while the player is within mChaseRange of home, toward home
 *      otherwise, runs the two particle emitters, and drops to state 2 when
 *      it gets home, loses the player, or mChaseTimer reaches 0x384.
 *   2  Tornado::State2 -- winding down. Shrinks over 0x3c frames,
 *      drops mCaughtActor and returns to state 0.
 * Behavior counts mStateTimer up every frame and zeroes it, along with
 * mTriggerCount, whenever the state changed.
 *
 * 0x080..0x0a0 IS dActor_c's OWN LAYOUT, not this class's, and is named from
 * include/dActor_c.h by offset -- the range comment below already said so for
 * the first eight fields, and mVertAccel/mTerminalVelocity are the last two.
 *
 * Provenance table: notes/butterfly-tornado-provenance.md. */
#ifndef TORNADO_H
#define TORNADO_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "TextureTransformer.h"

/* The ROM's RTTI names this class daTor_c and records dActor_c as its sole
 * base.  Tornado is the readable compatibility name already used by the
 * matched method symbols.  The vtable at 0x02137a90 has exactly the 31
 * dActor_c slots; Tornado overrides 0/3/6/9 and the destructor pair 16/17. */
struct Tornado : dActor_c {
    u8 pad_0d0[0x4];
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker whose pad stopped short of the object, so the
       member also takes over unk_0f4 (+0x20 = dCc_c::hitFlags) and unk_0f8
       (+0x24 = dCc_c::otherOwner), which Behavior reads as exactly those two. */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x108 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7TornadoD0Ev.cpp] */
    dBgCh_Actr mWithMeshClsn;            /* 0x108 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x2c4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x2c4 */
    /* TextureTransformer member, named by the class's own destructor calling
       TextureTransformer's D1 at +0x328 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7TornadoD0Ev.cpp] */
    TextureTransformer mTextureTransformer;            /* 0x328 */
    /* The actor that hit this tornado. Set from mdCcAc_c.otherOwner once
       func_ov002_020de33c approves it; Tornado::State2 clears it as
       the tornado winds down. */
    dActor_c *mCaughtActor;       /* 0x33c */
    /* Where the tornado belongs. InitResources copies mPos here; state 0 snaps
       mPos back to it after mChaseTimer runs out, and both other states
       measure their distances from it rather than from where the tornado
       currently is. */
    s32 mHomePosX;               /* 0x340 */
    s32 mHomePosY;               /* 0x344 */
    s32 mHomePosZ;               /* 0x348 */
    s32 mChaseRange;             /* 0x34c -- how far from home the player may
                                     be and still be chased. InitResources
                                     builds it out of mParam's low byte:
                                     (byte * 0x64000), or 0x5dc000 when the
                                     byte is 0xff. */
    u16 mStateTimer;             /* 0x350 -- frames in the current state.
                                     Behavior counts it up and zeroes it when
                                     mState changed. State 0 spins up over its
                                     first 0x3c, state 2 shrinks over its own
                                     0x3c and gives up at 0x168. */
    u16 unk_352;                 /* 0x352 -- zeroed by state 0 and read
                                     nowhere in the tree. No evidence, so no
                                     name. */
    u16 mChaseTimer;             /* 0x354 -- frames since the tornado woke,
                                     counted up by state 1 only. At 0x384 the
                                     chase is over: state 1 drops to state 2
                                     and state 0 uses the same threshold to
                                     decide it must teleport home. */
    s16 mAngleToHome;            /* 0x356 -- Vec3_HorzAngle(mPos, mHomePos),
                                     recomputed by state 1 every frame and
                                     steered toward when not chasing. */
    s16 mAngleToPlayer;          /* 0x358 -- the same angle to the closest
                                     player, steered toward while chasing. */
    s16 unk_35a;                 /* 0x35a -- zeroed by InitResources, read
                                     nowhere. No evidence, so no name. */
    s32 mState;                  /* 0x35c -- 0/1/2, the switch in Behavior;
                                     see the header comment. */
    u8  mTriggerCount;           /* 0x360 -- an event counter both live states
                                     use as a latch, zeroed by Behavior on any
                                     state change. State 0 bumps it when a
                                     player comes within 0x5dc000 and only
                                     spins up while it is non-zero; state 1
                                     bumps it when mCaughtActor satisfies
                                     func_ov002_020de328 and stops chasing the
                                     player once it is non-zero. */
    u8  pad_361[0x3];
    s32 mParticleHandle0;        /* 0x364 -- the handle Particle::System::New
                                     returns for effect 0x11f, fed back in as
                                     its own first argument by state 1 so the
                                     emitter is not restarted. */
    s32 mParticleHandle1;        /* 0x368 -- the same for effect 0x120. */
    s32 mSoundHandle;            /* 0x36c -- the handle Sound::PlayLong returns
                                     for sound 0x85, fed back in the same way
                                     by states 0 and 1. Was the header's
                                     trailing pad; Tornado_Spawn allocates
                                     0x370, so this is the last word. */
    /* Inline is load-bearing: the small forcing translation units emit the
     * ROM's D1 and D0 while objisolate discards their wrappers and D2. */
    virtual ~Tornado() {}

    virtual int InitResources();       /* slot  0 */
    virtual int CleanupResources();    /* slot  3 */
    virtual int Behavior();            /* slot  6 */
    virtual int Render();              /* slot  9 */

    /* Nonvirtual routines owned by the same recovered TU. The descriptive
     * names are decomp names; their member ownership is evidenced by their
     * implicit-this call shape and exclusive access to Tornado state. */
    void UpdateSpin(s32 scale);
    void State2();
    void State1();
    void State0();
};

typedef char Tornado_size_must_be_0x370[sizeof(Tornado) == 0x370 ? 1 : -1];

#endif
