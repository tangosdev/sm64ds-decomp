#ifndef HUD_H
#define HUD_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   dMeter_c -- dMeter_c_classInit (was HUD_Spawn), g_profile_METER (was _ZN3HUD9spawnDataE)
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

#ifdef __cplusplus

struct HUD : dBase_c {
    u8  pad_050[0x10];
    u16 mVsTimer;                     /* 0x060 */
    u8  pad_062[0x4];
    u16 mVsTimerY;                    /* 0x066 */
    s16 mHealthMeterY;                /* 0x068 */
    u16 mHealthMeterHoldTimer;        /* 0x06a */
    u16 mHealthTickTimer;             /* 0x06c */
    u8  pad_06e[0x2];
    s16 mStarCountX;                  /* 0x070 */
    u8  pad_072[0x1];
    u8 mHealthMeterState;             /* 0x073 */
    s8 mDigits[3];                    /* 0x074 */

    /* --- vtable --- */
    virtual ~HUD();

    /* Two overrides the cartridge proves and this header never declared -- and both
       are already decompiled and byte-matching in this tree, which is what makes
       them worth reading twice. _ZTV3HUD slot 0 pointed at fBase_c::InitResources
       and slot 6 at fBase_c::Behavior, while the ROM has
       ov002:_ZN3HUD13InitResourcesEv (0x020fda04) and ov002:_ZN3HUD8BehaviorEv
       (0x020fd7a4) -- src/_ZN3HUD13InitResourcesEv.cpp and src/_ZN3HUD8BehaviorEv.cpp
       compiled and matched all along; nothing pointed the vtable at them.
       No `virtual` keyword, matching the overrides beside it: a derived declaration
       of a base virtual overrides whether or not it repeats the word.
       Measured by tools/romdata_check.py, the only gate that reads vtable bytes --
       objisolate drops every non-.text section, so 106/106 is blind here. */
    s32 InitResources();
    s32 Behavior();
    int Render();
    s32 CleanupResources();
    void CalculateDigits(unsigned short value);
    void OnPendingDestroy();
    void RenderHealthMeter();
    void RenderRedCoins();
    void RenderSilverStars();
    void RenderStarCount();
    void RenderTimeTimer();
    void RenderVsTimer();
    void UpdateHealthMeter();

    /* Tail padding. The field span stops short of the real size: dMeter_c_classInit
       calls fBase_c::operator new(0x7c), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_078[0x4];      /* 0x078, to the ROM's 0x7c */
};

typedef char HUD_size_must_be_0x7c[sizeof(HUD) == 0x7c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct HUD {
    u8  pad_000[0x60];
    /* Seconds left on the VS-mode clock. RenderVsTimer splits it /10 and %10
       into two glyphs, and recolours to palette 0xb at <= 5 -- so the field is
       whole seconds, not ticks. Unsigned: the ROM reads it with ldrh and the
       <= 5 test is unsigned. */
    u16 mVsTimer;            /* 0x060 */
    u8  pad_062[0x4];
    /* Y for the VS timer's digits. Passed straight through as OAM::Render's y
       at every one of the seven call sites, while their x values are literals
       -- so the row is animated and the columns are fixed. */
    u16 mVsTimerY;            /* 0x066 */
    s16 mHealthMeterY;            /* 0x068 */
    u16 mHealthMeterHoldTimer;            /* 0x06a */
    u16 mHealthTickTimer;            /* 0x06c */
    u8  pad_06e[0x2];
    /* Right-hand edge of the star counter: RenderStarCount seeds its cursor
       here and steps LEFT by 9 per digit drawn, so the count grows leftwards
       from a fixed origin. It is a field rather than a constant because the
       three render paths share it and the cursor is consumed by value. */
    s16 mStarCountX;            /* 0x070 */
    u8  pad_072[0x1];
    u8  mHealthMeterState;            /* 0x073 */
    /* The three decimal digits CalculateDigits splits a count into, most
       significant first. SIGNED because -1 is the sentinel it writes for a
       leading zero that should not be drawn -- and it only suppresses at i<2,
       so a value of 0 still renders its final digit rather than nothing.
       RenderCoinCount reaches the same bytes through a `char pad[0x74]`
       shadow; that file is a separate slice. */
    s8  mDigits[3];            /* 0x074 */
};

#endif /* __cplusplus */

#endif /* HUD_H */
