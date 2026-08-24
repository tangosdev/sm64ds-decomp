#ifndef UNAGI_H
#define UNAGI_H

#include "types.h"

/* The eel. Its destructor is the layout, and every member closes exactly on
 * the next field:
 *
 *     dEnemyBase_c                     ends 0x110
 *     dCcAcPos_c 0x110 + 0x040 = 0x150  -> the second one
 *     dCcAcPos_c 0x150 + 0x040 = 0x190  -> dBgCh_Actr
 *     dBgCh_Actr              0x190 + 0x1bc = 0x34c  -> mState
 *     BlendModelAnim            0x350 + 0x070 = 0x3c0
 *     Vector3[7]                0x448 + 0x054 = 0x49c  -> mStarUniqueID
 *
 * unk_360 and unk_3ac are gone: both fall INSIDE the BlendModelAnim at 0x350
 * (+0x10 and +0x5c), so they were never Unagi's fields -- the generated header
 * declared them beside a `u8 mBlendModelAnim` marker whose padding stopped
 * short of the real object.
 *
 * The array at 0x448 is Vector3[7]: the ROM destroys it with
 * __destroy_arr(ptr, 7, 0xc, _ZN7Vector3D1Ev), and 0xc is sizeof(Vector3).
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "BlendModelAnim.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct Unagi : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c1;  /* 0x110 */
    dCcAcPos_c mdCcAcPos_c2;  /* 0x150 */
    dBgCh_Actr mWithMeshClsn;                             /* 0x190 */
    s32 mState;                                             /* 0x34c */
    BlendModelAnim mBlendModelAnim;                         /* 0x350 */
    s32 unk_3c0;                                            /* 0x3c0 */
    u8  pad_3c4[0x2c];
    s32 mHomePosX;                                          /* 0x3f0 */
    s32 mHomePosY;                                          /* 0x3f4 */
    s32 mHomePosZ;                                          /* 0x3f8 */
    u8  pad_3fc[0x8];
    s32 mPathID;                                            /* 0x404 */
    s32 mVariant;                                           /* 0x408 */
    s32 mPathNodeCount;                                     /* 0x40c */
    s32 mPathNodeIndex;                                     /* 0x410 */
    u8  mStarParam;                                         /* 0x414 */
    u8  pad_415[0x3];
    s16 mSegmentAngle[8];                                   /* 0x418 */
    s16 mInitAngleX;                                        /* 0x428 */
    s16 mInitAngleY;                                        /* 0x42a */
    s16 mInitAngleZ;                                        /* 0x42c */
    u8  pad_42e[0xe];
    Vector3 mStarPos;                                       /* 0x43c */
    Vector3 mSegmentPos[7];                                 /* 0x448 */
    s32 mStarUniqueID;                                      /* 0x49c */
    /* trailing extent the ROM's `new Unagi` literal proves; see tools/opnew_sizes.py */
    u8 pad_4a0[0x10];

    virtual ~Unagi();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    void OnPendingDestroy();
};

typedef char Unagi_size_must_be_0x4b0[sizeof(struct Unagi) == 0x4b0 ? 1 : -1];

#else

/* The same object for a C translation unit, flat -- Render and D0 are C files
   that read these fields, and D0 is compiler-generated so it can never be
   migrated. Same arrangement as include/dBgActor_c.h. */
struct Unagi {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x8];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x28];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x33];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mdCcAcPos_c1;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mdCcAcPos_c2;            /* 0x150 */
    u8  pad_151[0x3f];
    u8  mWithMeshClsn;            /* 0x190 */
    u8  pad_191[0x1bb];
    s32 mState;            /* 0x34c */
    u8  mBlendModelAnim;            /* 0x350 */
    u8  pad_351[0xf];
    s32 unk_360;            /* 0x360 */
    u8  pad_364[0x48];
    s32 unk_3ac;            /* 0x3ac */
    u8  pad_3b0[0x4];
    s32 unk_3b4;            /* 0x3b4 */
    u8  pad_3b8[0x38];
    s32 mHomePosX;            /* 0x3f0 */
    s32 mHomePosY;            /* 0x3f4 */
    s32 mHomePosZ;            /* 0x3f8 */
    u8  pad_3fc[0x8];
    s32 mPathID;            /* 0x404 */
    s32 mVariant;            /* 0x408 */
    s32 mPathNodeCount;            /* 0x40c */
    s32 mPathNodeIndex;            /* 0x410 */
    u8  mStarParam;            /* 0x414 */
    u8  pad_415[0x3];
    s16 mSegmentAngle[8];            /* 0x418 */
    s16 mInitAngleX;            /* 0x428 */
    s16 mInitAngleY;            /* 0x42a */
    s16 mInitAngleZ;            /* 0x42c */
    u8  pad_42e[0xe];
    s32 mStarPosX;            /* 0x43c */
    s32 mStarPosY;            /* 0x440 */
    s32 mStarPosZ;            /* 0x444 */
    u8  pad_448[0x54];
    s32 mStarUniqueID;            /* 0x49c */
    /* trailing extent the ROM's `new Unagi` literal proves; see tools/opnew_sizes.py */
    u8 pad_4a0[0x10];
};

#endif /* __cplusplus */

#endif /* UNAGI_H */
