#ifndef PRIVATE_CONTROLLER_INPUT_H
#define PRIVATE_CONTROLLER_INPUT_H

#include "types.h"

/* The per-player input records, four of each (gActivePlayerSlot indexes
 * them; see PlayerInput.h). Plain C types: some consumers are C.
 *
 * PadData is the 4-byte key word pair at data_020a0e58 (pressed is the
 * data_020a0e5a lane). dScStage_c::CheckInput reads both into the
 * controller record below.
 *
 * ControllerInput is the 0x18-byte record at data_0209f498. CheckInput
 * fills it from the pad and the touch screen every frame and ResetInput
 * clears it; SetControllerMode and InitControllerMode write the mode byte
 * (data_0209f4ae). The layout is CheckInput's; d_s_stage_methods.cpp still
 * carries it as its local PadData and CamInput. */
typedef struct PadData {
    u16 held;    /* 0x00 */
    u16 pressed; /* 0x02 */
} PadData;

typedef struct ControllerInput {
    s16 dx;      /* 0x00 */
    s16 dy;      /* 0x02 */
    u16 held;    /* 0x04 */
    u16 pressed; /* 0x06 */
    s16 mag;     /* 0x08 */
    s16 nx;      /* 0x0a */
    s16 ny;      /* 0x0c */
    s16 ang;     /* 0x0e */
    u8 tx;       /* 0x10 */
    u8 ty;       /* 0x11 */
    u8 delay;    /* 0x12 */
    u8 cnt;      /* 0x13 */
    u8 touching; /* 0x14 */
    u8 pad_15;
    u8 mode; /* 0x16 */
    u8 pad_17;
} ControllerInput;

typedef char ControllerInput_size_must_be_0x18[sizeof(ControllerInput) == 0x18 ? 1 : -1];

#endif
