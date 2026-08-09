/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HUD: 11 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HUD_H
#define HUD_H
#include "types.h"

/* fwd */
struct value;
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
#ifdef __cplusplus
    /* methods */
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
#endif
};

#endif
