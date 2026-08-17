#ifndef BIGBRICKBLOCK_H
#define BIGBRICKBLOCK_H

#include "types.h"

/* Derives from Platform: the destructor stores this class's vtable, then
 * Platform's -- inlined -- then destroys the MovingMeshCollider at 0x124 and
 * the Model at 0xd4 before chaining to Actor. All three belong to Platform.
 * Everything this header used to restate below 0x31e was Actor's and
 * Platform's, and is inherited now.
 *
 * ONE CLASS, FIVE ACTORS. BrickBlock, BigBrickBlock, BrickBlockSwitchActivated,
 * BlackBrickBlock and FortressTowerWall all have their own SpawnInfo, and all
 * five factories are the same three instructions: ActorBase::operator new(816),
 * Platform::Platform(), store _ZTV13BigBrickBlock. There is no second vtable and
 * no second size anywhere in the family, which is why every method here switches
 * on actorID (0xf, 0x10, 0x11, 0x12, 0x13, 0x2e) instead of being overridden.
 *
 * SIZE IS 0x330, THE LITERAL ALL FIVE FACTORIES PASS TO operator new -- not the
 * field span. It used to say 0x328, which is where the last field this header
 * knew about ended, and three separate places in the ROM read past it:
 *
 *   +0x322  Kill's `ldrb r0, [r4, #0x322]` and InitResources' star-ID write
 *   +0x323  Kill hands `r4 + 0x323` to Actor::UntrackAndSpawnStar as its
 *           `s8 &trackStarID`; InitResources stores Actor::TrackStar's result there
 *   +0x328  Kill's `ldr r0, [r4, #0x328]`, and func_ov002_020b363c dereferences
 *           the same word as an actor (reads actorID at +0xc)
 *   +0x32c  a u8 variant index, 0..2, written by InitResources and used by
 *           CleanupResources to stride the 0xc-byte file tables at
 *           data_ov002_02108ab0 / _ab4 / _ab8. Left as padding here: this change
 *           only needed the three fields above.
 *
 * The 0x322/0x323 pair was inside a pad_321[0x3] run and 0x328 was past the end
 * of the class, so slot 31 (Kill) could not be written as a method until now.
 */

#ifdef __cplusplus

#include "Platform.h"

struct BigBrickBlock : Platform {
    u8 unk_31e;                       /* 0x31e */
    u8 unk_31f;                       /* 0x31f */
    u8 mEventID;                      /* 0x320 */
    u8  pad_321[0x1];
    /* The star this block holds, low byte of param1, clamped: InitResources
       writes it and zeroes it when it reads 0xff. Kill spawns the star when it
       is non-zero. */
    u8 mStarID;                       /* 0x322 */
    /* The star-marker slot, s8 because Actor::TrackStar returns one and
       Actor::UntrackAndSpawnStar takes `s8 &` -- Kill passes `this + 0x323` as
       exactly that argument. */
    s8 mTrackStarID;                  /* 0x323 */
    Actor *mSwitch;                   /* 0x324 */
    /* An Actor *, and func_ov002_020b363c is the evidence: it loads this word
       and reads actorID at +0xc off it, then writes +0x3b0 or +0xd6. Kill only
       tests it against null. */
    Actor *unk_328;                   /* 0x328 */
    /* 0x32c is a u8 variant index; see the header comment. */
    u8  pad_32c[0x4];

    /* --- vtable --- */
    virtual ~BigBrickBlock();

    /* Slot 31, Platform's own new virtual (include/Platform.h). Attributed by
       the vtable: _ZTV13BigBrickBlock (ov002 0x02108adc) carries 0x020b38a0 at
       slot 31 -- vtable + 0x7c -- where _ZTV8Platform carries
       _ZN8Platform4KillEv, and slot 30 is Actor's 0x020100dc in both. An
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
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
    u8  unk_31f;            /* 0x31f */
    u8  mEventID;            /* 0x320 */
    u8  pad_321[0x1];
    u8  mStarID;            /* 0x322 */
    s8  mTrackStarID;            /* 0x323 */
    /* Actor * -- the ROM loads this WORD and passes it to _ZN5Actor15FindWithActorIDEjPS_
       as that function's `this`, which is an object address, so the word is a Actor *. It
       says nothing about the rest of the marker's span, which stays explicit padding. Was
       a u8 marker. */
    Actor *mSwitch;            /* 0x324 */
    Actor *unk_328;            /* 0x328 */
    u8  pad_32c[0x4];
};

#endif /* __cplusplus */

#endif /* BIGBRICKBLOCK_H */
