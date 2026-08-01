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
    u8  pad_000[0x68];
    s16 mHealthMeterY;            /* 0x068 */
    u16 mHealthMeterHoldTimer;            /* 0x06a */
    u16 mHealthTickTimer;            /* 0x06c */
    u8  pad_06e[0x5];
    u8  mHealthMeterState;            /* 0x073 */
#ifdef __cplusplus
    /* methods */
    int Render();
    s32 CleanupResources();
    void CalculateDigits(unsigned short value);
    void OnPendingDestroy();
    void RenderHealthMeter();
    void RenderStarCount();
    void RenderTimeTimer();
    void RenderVsTimer();
    void UpdateHealthMeter();
#endif
};

#endif
