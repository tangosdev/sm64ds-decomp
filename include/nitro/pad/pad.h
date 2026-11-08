#ifndef NITRO_PAD_PAD_H
#define NITRO_PAD_PAD_H

/*
 * NitroSDK's key bits (nitro/pad/common/pad.h). The game's PadData words
 * (include/private/controller_input.h) carry them in the SDK's layout, so
 * a key test reads as the SDK names. Macros only: a macro costs no
 * compiler-numbered symbols in the files that include it.
 */

#define PAD_BUTTON_A 0x0001
#define PAD_BUTTON_B 0x0002
#define PAD_BUTTON_SELECT 0x0004
#define PAD_BUTTON_START 0x0008
#define PAD_KEY_RIGHT 0x0010
#define PAD_KEY_LEFT 0x0020
#define PAD_KEY_UP 0x0040
#define PAD_KEY_DOWN 0x0080
#define PAD_BUTTON_R 0x0100
#define PAD_BUTTON_L 0x0200
#define PAD_BUTTON_X 0x0400
#define PAD_BUTTON_Y 0x0800

#endif /* NITRO_PAD_PAD_H */
