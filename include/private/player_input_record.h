#ifndef PRIVATE_PLAYER_INPUT_RECORD_H
#define PRIVATE_PLAYER_INPUT_RECORD_H

#include "nitro/spi/tp.h"

/* One frame's input as the link exchanges it, 0x24 bytes. func_0203df40
 * samples this console's into data_020a1040 every frame; data_020a1154 holds
 * the four players' (func_0203db64 clears all 0x90 bytes). Moved here from
 * func_0203dabc and func_0203db64, which each viewed one field of it; the
 * layout is func_0203df40's. Its accessors that start at a field's own symbol
 * (data_020a1160 flags, data_020a1162 keys, data_020a1164 inputYaw,
 * data_020a1166 settings, data_020a1052 this console's settings) keep their
 * one-field views. Plain C types: the consumers are C. */
typedef struct PlayerInputRecord {
    s32 frame;    /* 0x00 -- counted up once per frame by func_0203df40 */
    TPData touch; /* 0x04 -- the calibrated sample func_0203b9bc returns */
    /* 0x0c -- 0x8000 set every frame (func_0203df40), 0x4000 the load request
     * (func_0203d918, func_0203d930), 0x1000 the link handshake
     * (func_0203d868, func_0203d81c), bit 0 a handshake wait (func_0203db64) */
    u16 flags;
    u16 keys;          /* 0x0e -- pressed buttons */
    s16 inputYaw;      /* 0x10 -- the camera's input yaw (func_0203dafc) */
    u8 settings[0x10]; /* 0x12 -- the link settings bytes (GetPlayerFlagByte) */
    u8 pad_22[2];      /* 0x22 -- func_0203da4c clears the byte at 0x22 */
} PlayerInputRecord;

typedef char PlayerInputRecord_size_must_be_0x24[sizeof(PlayerInputRecord) == 0x24 ? 1 : -1];

extern PlayerInputRecord data_020a1040;    /* this console's */
extern PlayerInputRecord data_020a1154[4]; /* the four players' */

#endif
