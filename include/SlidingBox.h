/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SlidingBox: 6 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SLIDINGBOX_H
#define SLIDINGBOX_H
#include "types.h"
#include "Model.h"
#include "dBgCh_Actr.h"
#include "dBgW_KcMbg.h"

struct SlidingBox {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x2];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 mHorzSpeed;            /* 0x098 */
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    s32 unk_0a4;            /* 0x0a4 */
    s32 mVertSpeed;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x24];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~SlidingBox calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mShip;            /* 0x320 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x324 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10SlidingBoxD1Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x324 */
    s32 unk_4e0;            /* 0x4e0 */
    s32 unk_4e4;            /* 0x4e4 */
    s32 unk_4e8;            /* 0x4e8 */
    s32 mHorzPos;            /* 0x4ec */
    s32 mSoundID;            /* 0x4f0 */
    u8  mState;            /* 0x4f4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char SlidingBox_size_must_be_0x4f8[sizeof(struct SlidingBox) == 0x4f8 ? 1 : -1];

#endif
