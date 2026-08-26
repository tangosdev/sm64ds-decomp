#ifndef BIGBRICKBLOCK_H
#define BIGBRICKBLOCK_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * ONE CLASS, FIVE ACTORS. BrickBlock, BigBrickBlock, BrickBlockSwitchActivated,
 * BlackBrickBlock and FortressTowerWall all have their own SpawnInfo, and all
 * five factories are the same three instructions: fBase_c::operator new(816),
 * dBgActor_c::dBgActor_c(), store _ZTV13BigBrickBlock. There is no second vtable and
 * no second size anywhere in the family, which is why every method here switches
 * on actorID (0xf, 0x10, 0x11, 0x12, 0x13, 0x2e) instead of being overridden.
 *
 * SIZE IS 0x330, THE LITERAL ALL FIVE FACTORIES PASS TO operator new -- not the
 * field span. It used to say 0x328, which is where the last field this header
 * knew about ended, and three separate places in the ROM read past it:
 *
 *   +0x322  Kill's `ldrb r0, [r4, #0x322]` and InitResources' star-ID write
 *   +0x323  Kill hands `r4 + 0x323` to dActor_c::UntrackAndSpawnStar as its
 *           `s8 &trackStarID`; InitResources stores dActor_c::TrackStar's result there
 *   +0x328  Kill's `ldr r0, [r4, #0x328]`, and NotifyLinkedActor dereferences
 *           the same word as an actor (reads actorID at +0xc)
 *   +0x32c  a u8 variant index, 0..2, written by InitResources and used by
 *           CleanupResources to stride the 0xc-byte file tables at
 *           data_ov002_02108ab0 / _ab4 / _ab8.
 *
 * The 0x322/0x323 pair was inside a pad_321[0x3] run and 0x328 was past the end
 * of the class, so slot 31 (Kill) could not be written as a method until now.
 */

struct StarSwitch;

#ifdef __cplusplus

#include "dBgActor_c.h"

struct BigBrickBlock : dBgActor_c {
    /* Kill sets it on the switch-activated variant instead of destroying the
       block; Render draws nothing and Behavior runs the broken path while it is
       set, and Behavior clears it the frame Event::GetBit(mEventID) disagrees
       with mPrevEventBit. */
    u8 mBroken;                       /* 0x31e */
    u8 mPrevEventBit;                 /* 0x31f -- Event::GetBit(mEventID) as InitResources last saw it */
    u8 mEventID;                      /* 0x320 */
    u8  pad_321[0x1];
    /* The star this block holds, low byte of param1, clamped: InitResources
       writes it and zeroes it when it reads 0xff. Kill spawns the star when it
       is non-zero. */
    u8 mStarID;                       /* 0x322 */
    /* The star-marker slot, s8 because dActor_c::TrackStar returns one and
       dActor_c::UntrackAndSpawnStar takes `s8 &` -- Kill passes `this + 0x323` as
       exactly that argument. */
    s8 mTrackStarID;                  /* 0x323 */
    StarSwitch *mSwitch;                 /* 0x324 */
    /* A dActor_c *, and NotifyLinkedActor is the evidence: it loads this word
       and reads actorID at +0xc off it, then writes +0x3b0 or +0xd6. Kill only
       tests it against null. */
    dActor_c *mLinkedActor;                   /* 0x328 */
    /* THE VARIANT INDEX, out of the padding at last. InitResources derives it
       from actorID (0xf/0x10/0x13 -> 0, 0x11 -> 1, 0x2e -> 2, 0x12 leaves it),
       and then uses it as the row index into all three 0xc-stride resource
       tables at 0x02108ab0/ab4/ab8. One byte of pad became one u8, so nothing
       below it moves. */
    u8  mVariant;                     /* 0x32c */
    u8  pad_32d[0x3];

    /* --- vtable --- */
    virtual ~BigBrickBlock();

    /* Slot 31, dBgActor_c's own new virtual (include/dBgActor_c.h). Attributed by
       the vtable: _ZTV13BigBrickBlock (ov002 0x02108adc) carries 0x020b38a0 at
       slot 31 -- vtable + 0x7c -- where _ZTV10dBgActor_c carries
       _ZN10dBgActor_c4KillEv, and slot 30 is dActor_c's 0x020100dc in both. An
       override adds no slot and no field, so the size assert is unaffected.

       NOT the key function: ~BigBrickBlock() above is declared out of line and
       is defined as a real method by src/_ZN13BigBrickBlockD1Ev.cpp and
       src/_ZN13BigBrickBlockD0Ev.cpp, so the destructor stays the first
       non-inline virtual and those two TUs keep emitting _ZTV13BigBrickBlock.
       This one does not -- checked with objisolate, not assumed. */
    virtual void Kill();              /* slot 31 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Non-virtual helpers owned by this TU. NotifyLinkedActor has one caller,
       Kill(), and every object access is through this or mLinkedActor.
       HasNonzeroAngleX is called only by Kill and InitResources. */
    void NotifyLinkedActor();
    bool HasNonzeroAngleX();

    /* Slots 21-24 and 27, all dActor_c combat-callback overrides (see
       include/dActor_c.h for the slot table). Attributed by the vtable, not
       by the pre-migration `recovered name:` comments, which name the wrong
       class one level down the tree (actor-class-names-off-by-one). None of
       these is the key function: ~BigBrickBlock() above stays the first
       out-of-line virtual, so these five TUs do not emit _ZTV13BigBrickBlock
       -- checked with objisolate, not assumed.

       OnKicked returns `void`, not `int`: this override is what proved
       include/dActor_c.h's slot 24 declaration wrong (it has two locals and
       two early returns, and mwcc allocates registers differently for `int`
       vs `void` even though r0 is never touched) -- measured with
       tools/mangle.py, not assumed. OnGroundPounded and OnHitByMegaChar are
       also `void`, matching the slot 21/27 corrections Stump forced
       (include/Stump.h) -- both happened to byte-match under the old `int`
       here too, so the correction is a no-op for this class, re-verified
       rather than assumed. */
    void OnGroundPounded(dActor_c &other);  /* slot 21 */
    int OnAttacked1(dActor_c &other);       /* slot 22 */
    int OnAttacked2(dActor_c &other);       /* slot 23 */
    void OnKicked(dActor_c &other);         /* slot 24 */
    void OnHitByMegaChar(Player &player);   /* slot 27 */
};

typedef char BigBrickBlock_size_must_be_0x330[sizeof(BigBrickBlock) == 0x330 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct BigBrickBlock {
    u8  pad_000[0xc];
    u16 mActorId;            /* 0x00c */
    u8  pad_00e[0xc6];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13BigBrickBlockD1Ev.c] */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~BigBrickBlock calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x32];
    u8  mBroken;            /* 0x31e */
    u8  mPrevEventBit;            /* 0x31f */
    u8  mEventID;            /* 0x320 */
    u8  pad_321[0x1];
    u8  mStarID;            /* 0x322 */
    s8  mTrackStarID;            /* 0x323 */
    /* StarSwitch * -- Behavior follows dActor_c::FindWithActorID(0xb, ...)
       until the returned actor's byte at +0x34e equals mEventID. StarSwitch's
       independently recovered mEventBit is exactly +0x34e; Render also reads
       that same object's mTimer/mTimeLimit at +0x338/+0x33a. */
    struct StarSwitch *mSwitch;   /* 0x324 */
    dActor_c *mLinkedActor;            /* 0x328 */
    u8  pad_32c[0x4];
};

#endif /* __cplusplus */

#endif /* BIGBRICKBLOCK_H */
