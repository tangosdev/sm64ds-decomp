/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Dorrie: 5 matched functions, 27 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DORRIE_H
#define DORRIE_H
#include "types.h"
#include "ModelAnim.h"
#include "dBgCh_Actr.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"

struct Dorrie {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    s32 mCap;            /* 0x0d4 */
    s32 unk_0d8;            /* 0x0d8 */
    s32 unk_0dc;            /* 0x0dc */
    s32 unk_0e0;            /* 0x0e0 */
    u8  pad_0e4[0x4];
    u8  mHasCap;            /* 0x0e8 */
    u8  pad_0e9[0x3];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xec -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       ran 0xe00 bytes PAST the end of the object; that space is not evidenced and stays
       explicit padding rather than being folded into the member. The destructor's
       __destroy_arr((char *)c + 0x150, 7, 0x200, ...) runs over exactly this range -- 7
       objects of 0x200 bytes, starting where the ModelAnim ends -- so the extent is
       accounted for even though nothing names the element type. */
    ModelAnim mModelAnim;            /* 0x0ec */
    u8  pad_150[0xe00];
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0xf50 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6DorrieD1Ev.cpp] */
    dBgCh_Actr mWithMeshClsn;            /* 0xf50 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x110c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6DorrieD1Ev.cpp] */
    dCcAc_c mCylClsn1;           /* 0x110c */
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x1140 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6DorrieD1Ev.cpp] */
    dCcAcPos_c mCylClsn2;           /* 0x1140 */
    s32 mHomePosX;           /* 0x1180 */
    s32 mHomePosY;           /* 0x1184 */
    s32 mHomePosZ;           /* 0x1188 */
    s32 mClsnPlayer;           /* 0x118c */
    s32 mRider;           /* 0x1190 */
    s32 mSpawnPosX;           /* 0x1194 */
    s32 mSpawnPosY;           /* 0x1198 */
    s32 mSpawnPosZ;           /* 0x119c */
    u8  pad_11a0[0x8];
    s32 mPushDownHeight;           /* 0x11a8 */
    s32 mSinkHeight;           /* 0x11ac */
    u8  pad_11b0[0x2];
    s16 mStateTimer;           /* 0x11b2 */
    u8  pad_11b4[0x1];
    u8  mClsnState;           /* 0x11b5 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char Dorrie_size_must_be_0x11b8[sizeof(struct Dorrie) == 0x11b8 ? 1 : -1];

#endif
