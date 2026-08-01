/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class DorrieCap: 5 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DORRIECAP_H
#define DORRIECAP_H
#include "types.h"

struct DorrieCap {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x20];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  unk_0d4;            /* 0x0d4 */
    u8  pad_0d5[0x1a];
    u8  unk_0ef;            /* 0x0ef */
    u8  mModel;            /* 0x0f0 */
    u8  pad_0f1[0x4f];
    u8  mMovingCylinderClsn;            /* 0x140 */
    u8  pad_141[0x33];
    s32 unk_174;            /* 0x174 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
