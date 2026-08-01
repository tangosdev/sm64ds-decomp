/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SnowmanBreath: 5 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SNOWMANBREATH_H
#define SNOWMANBREATH_H
#include "types.h"

struct SnowmanBreath {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x1304];
    u8  unk_1394;           /* 0x1394 */
    u8  pad_1395[0x2f];
    u8  unk_13c4;           /* 0x13c4 */
    u8  pad_13c5[0x7];
    s32 unk_13cc;           /* 0x13cc */
    u8  unk_13d0;           /* 0x13d0 */
    u8  pad_13d1[0x1];
    u8  unk_13d2;           /* 0x13d2 */
    u8  unk_13d3;           /* 0x13d3 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
