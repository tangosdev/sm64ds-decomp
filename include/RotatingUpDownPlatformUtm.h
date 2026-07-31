/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RotatingUpDownPlatformUtm: 6 matched functions, 30 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROTATINGUPDOWNPLATFORMUTM_H
#define ROTATINGUPDOWNPLATFORMUTM_H
#include "types.h"

struct RotatingUpDownPlatformUtm {
    u8  pad_000[0x8];
    s32 mSpawnParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x17];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x1e];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1db];
    u8  unk_300;            /* 0x300 */
    u8  pad_301[0x1f];
    u8  mShadowModel;            /* 0x320 */
    u8  pad_321[0x57];
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    s16 unk_380;            /* 0x380 */
    s16 unk_382;            /* 0x382 */
    s16 unk_384;            /* 0x384 */
    u8  pad_386[0x2];
    s32 mBasePos;            /* 0x388 */
    s32 unk_38c;            /* 0x38c */
    s32 unk_390;            /* 0x390 */
    u8  mWaypointIndex;            /* 0x394 */
    u8  mVariant;            /* 0x395 */
    u8  pad_396[0x2];
    u8  mPlatform0;            /* 0x398 */
    u8  pad_399[0x3];
    u8  mPlatform1;            /* 0x39c */
    u8  pad_39d[0x3];
    u8  unk_3a0;            /* 0x3a0 */
    u8  pad_3a1[0x1];
    u8  unk_3a2;            /* 0x3a2 */
    u8  pad_3a3[0x1];
    s16 unk_3a4;            /* 0x3a4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
