/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Actor: 72 matched functions, 34 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ACTOR_H
#define ACTOR_H
#include "types.h"

/* fwd */
struct Player;
struct Vector3;
struct a;
struct b;
struct player_;
struct pos_;
struct Actor {
    u8  pad_000[0x4];
    u8  unk_004;            /* 0x004 */
    u8  pad_005[0x7];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x42];
    s32 unk_050;            /* 0x050 */
    s32 unk_054;            /* 0x054 */
    u8  unk_058;            /* 0x058 */
    u8  pad_059[0x3];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    s32 unk_068;            /* 0x068 */
    s32 unk_06c;            /* 0x06c */
    s32 unk_070;            /* 0x070 */
    s32 mCamSpacePosX;            /* 0x074 */
    s32 mCamSpacePosY;            /* 0x078 */
    s32 mCamSpacePosZ;            /* 0x07c */
    u8  pad_080[0xc];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  unk_098;            /* 0x098 */
    u8  pad_099[0x3];
    u8  unk_09c;            /* 0x09c */
    u8  pad_09d[0x3];
    u8  unk_0a0;            /* 0x0a0 */
    u8  pad_0a1[0x3];
    u8  unk_0a4;            /* 0x0a4 */
    u8  pad_0a5[0x3];
    u8  unk_0a8;            /* 0x0a8 */
    u8  pad_0a9[0x3];
    u8  unk_0ac;            /* 0x0ac */
    u8  pad_0ad[0x3];
    u32 mFlags;            /* 0x0b0 */
    s32 unk_0b4;            /* 0x0b4 */
    s32 unk_0b8;            /* 0x0b8 */
    s32 unk_0bc;            /* 0x0bc */
    s32 unk_0c0;            /* 0x0c0 */
    u8  unk_0c4;            /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;            /* 0x0ce */
#ifdef __cplusplus
    /* methods */
    int BeforeBehavior();
    int BumpedUnderneathByPlayer(Player & player_);
    int GetSubtraction(short a, short b);
    void HugeLandingDustAt(Vector3 & pos_, bool b);
    void LandingDustAt(Vector3 & pos_, bool b);
#endif
};

#endif
