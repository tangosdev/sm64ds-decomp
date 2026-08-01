/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Platform: 9 matched functions, 24 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PLATFORM_H
#define PLATFORM_H
#include "types.h"

/* fwd */
struct Player;
struct player_;
struct Platform {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x19];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 unk_0b0;            /* 0x0b0 */
    s32 unk_0b4;            /* 0x0b4 */
    s32 unk_0b8;            /* 0x0b8 */
    u8  pad_0bc[0x18];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x23];
    s32 unk_114;            /* 0x114 */
    s32 unk_118;            /* 0x118 */
    s32 unk_11c;            /* 0x11c */
    u8  pad_120[0x4];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1eb];
    s32 unk_310;            /* 0x310 */
    s32 unk_314;            /* 0x314 */
    s32 unk_318;            /* 0x318 */
    u8  unk_31c;            /* 0x31c */
    u8  unk_31d;            /* 0x31d */
#ifdef __cplusplus
    /* methods */
    void KillByMegaChar(Player & player_);
    void UpdateClsnPosAndRot();
    void UpdateModelPosAndRotY();
#endif
};

#endif
