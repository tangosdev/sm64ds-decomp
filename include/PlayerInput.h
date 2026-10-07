#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Per-player touch-screen state, rebuilt every frame by func_0203bb60
 * (cleared by func_0203bb14). Four players, one 4-byte record each:
 *
 *   +0 gTouchHeld   stylus is down this frame
 *   +1 gTouchEdge   held-state changed this frame (old ^ new): with gTouchHeld
 *                   set it means "touch just began", with gTouchHeld clear
 *                   "touch just ended"
 *   +2 gTouchX      touch x, truncated to u8
 *   +3 gTouchY      touch y, truncated to u8
 *
 * Most readers load each byte lane through its own literal-pool address
 * (0x...de8, de9, dea, deb) and index it as a flat byte array with a stride
 * of 4: gTouchX[slot * 4], not gTouchX[slot]. Some read the whole record
 * through gTouchHeld plus an offset, via a local struct overlay.
 *
 * gActivePlayerSlot is the player (0..3) the single-player code paths read.
 * Player::Behavior sets it to mPlayerNo while that player updates (or 0, per
 * data_0209fc48) and, once the update is done, to data_0209f250 (or 0 if
 * data_0209fc68 is clear). It indexes the stride-0x18 controller records at
 * data_0209f498 and the stride-4 key words at data_020a0e58/e5a.
 */
extern u8 gTouchHeld[];
extern u8 gTouchEdge[];
extern u8 gTouchX[];
extern u8 gTouchY[];
extern u8 gActivePlayerSlot;

#ifdef __cplusplus
}
#endif

#endif
