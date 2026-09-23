#ifndef DMETER_C_H
#define DMETER_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the class itself
 * -- _ZTS8dMeter_c at ov002:0x0210c218 stores the literal string "8dMeter_c",
 * and _ZTI8dMeter_c at 0x0210c224 points its name word there and its base word
 * at arm9:0x02086e78, which is _ZTI7dBase_c. The registry profile object, the
 * factory spelling and the member and field names below are Tier B
 * reconstructions -- evidence-bounded proposals, not recovered SM64DS symbols.
 * Exact original spellings are not preserved.
 *
 *   dMeter_c -- dMeter_c_classInit (was HUD_Spawn), g_profile_METER (was _ZN8dMeter_c9spawnDataE)
 */

#include "types.h"
#include "dBase_c.h"

/* Derives from dBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x50 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */
struct dMeter_c : dBase_c {
    u8  pad_050[0x10];
    /* Seconds left on the VS-mode clock. RenderVsTimer splits it by 10 into two
       glyphs and recolours to palette 0xb at <= 5, so the field is whole
       seconds, not ticks. Unsigned: the ROM reads it with ldrh and the <= 5
       test is unsigned. */
    u16 mVsTimer;                     /* 0x060 */
    /* Written once by InitResources -- 0x062 cleared, 0x064 seeded with 0xb4 --
       and never read back anywhere in this translation unit, so neither has
       earned a name. They keep the placeholder spelling the shards used, at
       their measured widths, because InitResources writes them by name. */
    s16 unk62;                        /* 0x062 */
    s16 unk64;                        /* 0x064 */
    /* Y for the VS timer's digits. Passed straight through as OAM::Render's y
       at every one of the seven call sites, while their x values are literals
       -- so the row is animated and the columns are fixed. */
    u16 mVsTimerY;                    /* 0x066 */
    s16 mHealthMeterY;                /* 0x068 */
    u16 mHealthMeterHoldTimer;        /* 0x06a */
    u16 mHealthTickTimer;             /* 0x06c */
    /* Left-hand edge of the life counter, and the exact mirror of mStarCountX
       below: Behavior drives the pair together, stepping this one +4 toward
       0x10 while that one moves -4 toward 0xf0 in the same branch, and
       InitResources seeds both in the same five-way ladder. RenderLifeCount
       consumes it as the row's x origin, then draws OAM::TIMES at +0x10 and the
       digits from +0x18. Signed because the parked value is -0x3a, off the left
       edge of the screen. */
    s16 mLifeCountX;                  /* 0x06e */
    /* Right-hand edge of the star counter: RenderStarCount seeds its cursor
       here and steps LEFT by 9 per digit drawn, so the count grows leftwards
       from a fixed origin. It is a field rather than a constant because the
       three render paths share it and the cursor is consumed by value. */
    s16 mStarCountX;                  /* 0x070 */
    /* Cleared beside mHealthMeterState on the eight-health branch of
       InitResources and never read back here, so it keeps the placeholder
       spelling too. */
    u8  unk72;                        /* 0x072 */
    u8  mHealthMeterState;            /* 0x073 */
    /* The three decimal digits CalculateDigits splits a count into, most
       significant first. SIGNED because -1 is the sentinel it writes for a
       leading zero that should not be drawn -- and it only suppresses at i<2,
       so a value of 0 still renders its final digit rather than nothing. */
    s8  mDigits[3];                   /* 0x074 */
    u8  pad_077[0x1];

    /* --- vtable --- */
    /* DEFINED INLINE, the same shape dBase_c uses and for the same reason: an
       inline body is what makes mwccarm emit D1 ahead of D0 and emit no D2 at
       all, which is the order the cartridge records at 0x020fb8f8 and
       0x020fb928. It also moves the key function down to InitResources, the
       first virtual below it that is left out of line -- and that one is defined
       in this class's own translation unit, so the vtable and the RTTI chain are
       still emitted there. */
    virtual ~dMeter_c() {}

    /* Two overrides the cartridge proves and this header never declared -- and both
       are already decompiled and byte-matching in this tree, which is what makes
       them worth reading twice. _ZTV8dMeter_c slot 0 pointed at fBase_c::InitResources
       and slot 6 at fBase_c::Behavior, while the ROM has
       ov002:_ZN8dMeter_c13InitResourcesEv (0x020fda04) and ov002:_ZN8dMeter_c8BehaviorEv
       (0x020fd7a4).
       No `virtual` keyword, matching the overrides beside it: a derived declaration
       of a base virtual overrides whether or not it repeats the word.
       Measured by tools/romdata_check.py, the only gate that reads vtable bytes --
       objisolate drops every non-.text section, so 106 of 106 is blind here. */
    s32 InitResources();
    s32 Behavior();
    int Render();
    s32 CleanupResources();
    void CalculateDigits(unsigned short value);
    void OnPendingDestroy();

    /* The non-virtual half of the HUD. None of these appears in _ZTV8dMeter_c,
       whose eighteen slots hold only the six overrides above and the two
       destructor variants, so every call to them from inside the class's own
       translation unit is the direct bl the ROM records. */
    void RenderCameraButtons();
    void RenderCoinCount();
    void RenderHealthMeter();
    void RenderLifeCount();
    void RenderRedCoins();
    void RenderSilverStars();
    void RenderStarCount();
    void RenderTimeTimer();
    void RenderVsTimer();
    void UpdateHealthMeter();
    void UpdateVsTimer();

    /* One-shot latch for the star counter's slide. InitResources sets it only on
       the branch that also parks mLifeCountX off-screen because the level
       carries no star display; Behavior's state 6 then reads it as the second
       arm of the test that pulls mStarCountX back to 0xf0, and clears it once
       both edges have arrived. */
    u8 mStarCountSlideIn;             /* 0x078 */

    /* Tail padding. The field span stops short of the real size: dMeter_c_classInit
       calls fBase_c::operator new(0x7c), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_079[0x3];
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char HUD_size_must_be_0x7c[sizeof(dMeter_c) == 0x7c ? 1 : -1];
#endif

#endif /* DMETER_C_H */
