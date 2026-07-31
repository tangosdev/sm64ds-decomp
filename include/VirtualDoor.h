/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class VirtualDoor: 4 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef VIRTUALDOOR_H
#define VIRTUALDOOR_H
#include "types.h"

struct VirtualDoor {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    u16 unk_090;            /* 0x090 */
    u8  pad_092[0x6];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x38];
    u8  unk_0d4;            /* 0x0d4 */
};

#endif
