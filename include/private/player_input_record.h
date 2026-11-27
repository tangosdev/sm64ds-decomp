#ifndef PRIVATE_PLAYER_INPUT_RECORD_H
#define PRIVATE_PLAYER_INPUT_RECORD_H

#include "nitro/spi/tp.h"

/* One frame's input as the link exchanges it, 0x24 bytes. func_0203df40
 * samples this console's into data_020a1040 every frame; data_020a1154 holds
 * the four players' (func_0203db64 clears all 0x90 bytes). The layout is
 * func_0203df40's. Every user is in src/engine/link/unit0203d7b8.cpp. */
typedef struct PlayerInputRecord {
    s32 frame;    /* 0x00 -- counted up once per frame by func_0203df40 */
    TPData touch; /* 0x04 -- the calibrated sample func_0203b9bc returns */
    /* 0x0c -- 0x8000 set every frame (func_0203df40), 0x4000 the load request
     * (func_0203d918, func_0203d930), 0x1000 the link handshake
     * (func_0203d868, func_0203d81c), bit 0 a handshake wait (func_0203db64) */
    u16 flags;
    u16 keys;          /* 0x0e -- pressed buttons */
    u16 inputYaw;      /* 0x10 -- the camera's input yaw (func_0203dafc) */
    /* 0x12 -- the link settings bytes (GetLinkByte, SetLinkByte);
     * the last is the desync check (LINK_SYNC_POSITION, LINK_SYNC_WINGS) */
    u8 settings[0x11];
    u8 unk23; /* 0x23 -- never read or written */
} PlayerInputRecord;

typedef char PlayerInputRecord_size_must_be_0x24[sizeof(PlayerInputRecord) == 0x24 ? 1 : -1];

/* The settings bytes the link checks for desync. dScStage_c::Render stores
 * two values through SetLinkByte every frame, an XOR of the players'
 * positions and a bit per player with wings, and func_0203ea5c is meant to
 * compare each across the four records, so that a position mismatch would
 * raise link error bit 0x08 and a wing mismatch 0x10. ClearLinkErrors and
 * func_0203ea5c's error countdown clear both.
 *
 * The names are coined. On the cartridge both indices are 0x10: Render's
 * second store overwrites its first, the two compares test the same byte,
 * and the two clears clear it twice. One wrong constant explains all three
 * doublings, so this is most likely an EAD bug, the second index presumably
 * meant to name another byte. What the cartridge does is a wing check only:
 * the position check never runs, and since a wing mismatch already fails the
 * first compare, error bit 0x10 can never be raised (callers test it through
 * GetLinkStatus(0x9d)).
 *
 * Which byte was meant is not recoverable. It was not 0x11: the link
 * exchanges only the first 0x11 settings bytes (0x12..0x22), which fill the
 * last 0x11 bytes of the 0x20-byte packet data_020a1020, and 0x20 bytes per
 * console is what func_0203fa50 shares, so record byte 0x23 had no room to
 * travel. Settings bytes 5..0x0f are never used and would have fit. */
#define LINK_SYNC_POSITION 0x10
#define LINK_SYNC_WINGS    0x10 /* presumably meant to differ (see above) */

extern PlayerInputRecord data_020a1040;    /* this console's */
extern PlayerInputRecord data_020a1154[4]; /* the four players' */

#endif
