/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Crate: 6 matched functions, 35 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CRATE_H
#define CRATE_H
#include "types.h"

/* Player is only ever pointed at from here, so a declaration is enough --
 * no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef PLAYER_FWD_DECLARED
#define PLAYER_FWD_DECLARED
struct Player;
typedef struct Player Player;
#endif

struct Crate {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
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
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x1c];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mWithMeshClsn;            /* 0x320 */
    u8  pad_321[0x1c7];
    s32 unk_4e8;            /* 0x4e8 */
    s32 unk_4ec;            /* 0x4ec */
    s32 unk_4f0;            /* 0x4f0 */
    u8  pad_4f4[0xc];
    s16 unk_500;            /* 0x500 */
    s16 unk_502;            /* 0x502 */
    s16 unk_504;            /* 0x504 */
    u8  pad_506[0x2];
    u8  mShadowModel;            /* 0x508 */
    u8  pad_509[0x57];
    s32 unk_560;            /* 0x560 */
    u8  mMovingCylinderClsnWithPos1;            /* 0x564 */
    u8  pad_565[0x33];
    s32 unk_598;            /* 0x598 */
    s32 unk_59c;            /* 0x59c */
    s32 unk_5a0;            /* 0x5a0 */
    u8  mMovingCylinderClsnWithPos2;            /* 0x5a4 */
    u8  pad_5a5[0x33];
    s32 unk_5d8;            /* 0x5d8 */
    s32 unk_5dc;            /* 0x5dc */
    s32 unk_5e0;            /* 0x5e0 */
    /* Player * -- the ROM loads this WORD and passes it to _ZN6Player9DropActorEv as that
       function's `this`, which is an object address, so the word is a Player *. It says
       nothing about the rest of the marker's span, which stays explicit padding. Was a u8
       marker. */
    Player *unk_5e4;            /* 0x5e4 */
    u8  pad_5e8[0xc];
    s32 unk_5f4;            /* 0x5f4 */
    u8  pad_5f8[0x4];
    u32 unk_5fc;            /* 0x5fc */
    u32 unk_600;            /* 0x600 */
    u8  pad_604[0x2];
    u8  unk_606;            /* 0x606 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
