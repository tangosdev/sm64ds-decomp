/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Bullet: 5 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BULLET_H
#define BULLET_H
#include "types.h"

struct Bullet {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x82];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x6];
    /* Actor::mVertAccel -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    /* Actor::mVertSpeed -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_0a8;            /* 0x0a8 */
    u8  unk_0ac;            /* 0x0ac */
    u8  pad_0ad[0x53];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModel;            /* 0x300 */
    u8  pad_301[0x57];
    s32 unk_358;            /* 0x358 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
