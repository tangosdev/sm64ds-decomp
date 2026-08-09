/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ChiefChilly: 5 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CHIEFCHILLY_H
#define CHIEFCHILLY_H
#include "types.h"
#include "BlendModelAnim.h"

struct ChiefChilly {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    s32 unk_068;            /* 0x068 */
    s32 unk_06c;            /* 0x06c */
    s32 unk_070;            /* 0x070 */
    /* Camera-space position: Behavior hands `self + 0x74` to func_02012694,
       which reads a Vector3 there. 0x068-0x070 is the triple it restores mPos
       FROM when a ground ray misses -- see the .cpp. */
    s32 mCamSpacePosX;            /* 0x074 */
    s32 mCamSpacePosY;            /* 0x078 */
    s32 mCamSpacePosZ;            /* 0x07c */
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x50];
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    /* BlendModelAnim member, named by _ZN14BlendModelAnimD1Ev at +0x30c -- a relocation
       the ROM build checks. D1 and not D2, so it is this type and not an inlined base. The
       marker's pad stopped short of the object, so the member also takes over unk_368
       (+0x5c = speed), which the header declared separately inside it. */
    BlendModelAnim mBlendModelAnim;            /* 0x30c */
    /* Current state: a record whose second word gates the tick and whose
       THIRD is the pointer-to-member Behavior calls. Behavior also compares
       it by ADDRESS against twelve file-scope state objects to gate five
       separate things. */
    void* mState;            /* 0x37c */
    u8  mShadowModel;            /* 0x380 */
    u8  pad_381[0x57];
    s32 unk_3d8;            /* 0x3d8 */
    s32 unk_3dc;            /* 0x3dc */
    s32 unk_3e0;            /* 0x3e0 */
    u8  pad_3e4[0xd8];
    s32 unk_4bc;            /* 0x4bc */
    u8  pad_4c0[0x5];
    u8  unk_4c5;            /* 0x4c5 */
    u8  pad_4c6[0x3];
    /* Set when a downward ray finds nothing AND the fall is fast enough;
       cleared whenever the ray hits. */
    u8  unk_4c9;            /* 0x4c9 */
    u8  pad_4ca[0x1];
    u8  unk_4cb;            /* 0x4cb */
    u8  pad_4cc[0x20];
    /* Last position before a failed ground ray rewound mPos. Written every
       time the ray misses, so it is the position the rewind came FROM.
       These three sit PAST where this header used to end (0x4cc); Behavior
       writes them, so the struct is at least 0x4f8. */
    s32 unk_4ec;            /* 0x4ec */
    s32 unk_4f0;            /* 0x4f0 */
    s32 unk_4f4;            /* 0x4f4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

#endif
