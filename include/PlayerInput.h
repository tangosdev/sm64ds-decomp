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
 *   +1 gTouchEdge   held-state changed this frame (old ^ new); readers pair it
 *                   with gTouchHeld to mean "touch just began"
 *   +2 gTouchX      touch x, truncated to u8
 *   +3 gTouchY      touch y, truncated to u8
 *
 * The ROM loads each byte lane through its own literal-pool address (0x...de8,
 * de9, dea, deb), so consumers index each lane as a flat byte array with a
 * stride of 4: gTouchX[slot * 4], not gTouchX[slot].
 *
 * gActivePlayerSlot is the player (0..3) the single-player code paths read:
 * 0 in single player, Player::mPlayerNo while a player's Behavior runs. It
 * also indexes the stride-0x18 controller array at data_0209f4a4.
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
