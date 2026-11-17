#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Per-player touch-screen state, rebuilt every frame by func_0203bb60
 * (cleared by func_0203bb14): gTouchInfo, one TouchInfo per player.
 *
 * A read of one field at a variable slot, gTouchInfo[slot].x, puts the
 * field's address in its own literal-pool word (gTouchInfo+2) and scales the
 * slot in the addressing mode; a read through a TouchInfo pointer adds the
 * slot once and takes each field at its offset. Readers use both shapes.
 *
 * gActivePlayerSlot is the player (0..3) the single-player code paths read.
 * daPly_c::Behavior sets it to mPlayerNo while that player updates (or 0, per
 * gRunningKuppaScript) and, once the update is done, to gLocalPlayerId (or 0 if
 * data_0209fc68 is clear). It indexes the stride-0x18 controller records at
 * data_0209f498 and the stride-4 key words at data_020a0e58/e5a.
 */

/* One player's touch record. */
typedef struct TouchInfo {
    u8 held; /* 0x0: stylus is down this frame */
    u8 edge; /* 0x1: held changed this frame (old ^ new): with held set the
                touch just began, with held clear it just ended */
    u8 x;    /* 0x2: touch x, truncated to u8 */
    u8 y;    /* 0x3: touch y, truncated to u8 */
} TouchInfo;

extern TouchInfo gTouchInfo[4];
extern u8 gActivePlayerSlot;

#ifdef __cplusplus
}
#endif

#endif
