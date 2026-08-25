/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Flamethrower: 3 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLAMETHROWER_H
#define FLAMETHROWER_H
#include "types.h"

struct Flamethrower {
    u8  pad_000[0x5c];
    /* 0x05c..0x08c is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mPosX;                 /* 0x05c */
    s32 mPosY;                   /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    s32 mPrevPosX;                 /* 0x068 */
    s32 mPrevPosY;                 /* 0x06c */
    s32 mPrevPosZ;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    /* TODO: Behavior reads past this generated span:
       dCc_c mClsn[12] at 0x0d4 (stride 0x3c),
       Vector3 mPartPos[12] at 0x3a4 (stride 0xc),
       Matrix4x3 at 0x434, then u8 state 0x464 / u8 active 0x465 /
       u16 timer 0x466 / u32 sound handle 0x468. */
    /* trailing extent the ROM's `new Flamethrower` literal proves; see tools/opnew_sizes.py */
    u8 pad_094[0x3d8];
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Behavior();
#endif
};

typedef char Flamethrower_size_must_be_0x46c[sizeof(struct Flamethrower) == 0x46c ? 1 : -1];

#endif
