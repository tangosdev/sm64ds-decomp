/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CameraTag: 4 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CAMERATAG_H
#define CAMERATAG_H
#include "types.h"

struct CameraTag {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x23];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    u8  pad_088[0x4];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
