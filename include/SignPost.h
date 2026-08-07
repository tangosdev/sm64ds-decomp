/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SignPost: 5 matched functions, 24 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SIGNPOST_H
#define SIGNPOST_H
#include "types.h"

/* Player is only ever pointed at from here, so a declaration is enough --
 * no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef PLAYER_FWD_DECLARED
#define PLAYER_FWD_DECLARED
struct Player;
typedef struct Player Player;
#endif

struct SignPost {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0xa];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    u8  mMovingCylinderClsn;            /* 0x320 */
    u8  pad_321[0x37];
    u8  mShadowModel;            /* 0x358 */
    u8  pad_359[0x57];
    s32 unk_3b0;            /* 0x3b0 */
    s32 unk_3b4;            /* 0x3b4 */
    s32 unk_3b8;            /* 0x3b8 */
    s16 unk_3bc;            /* 0x3bc */
    s16 unk_3be;            /* 0x3be */
    s16 unk_3c0;            /* 0x3c0 */
    u8  pad_3c2[0x6];
    u8  mWithMeshClsn;            /* 0x3c8 */
    u8  pad_3c9[0x1c5];
    s8  unk_58e;            /* 0x58e */
    u8  pad_58f[0x1];
    u8  unk_590;            /* 0x590 */
    u8  pad_591[0xb];
    /* Player * -- the ROM loads this WORD and passes it to _ZN6Player9DropActorEv as that
       function's `this`, which is an object address, so the word is a Player *. It says
       nothing about the rest of the marker's span, which stays explicit padding. Was a u8
       marker. */
    Player *unk_59c;            /* 0x59c */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
