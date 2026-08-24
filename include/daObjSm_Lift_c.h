#ifndef DAOBJSM_LIFT_C_H
#define DAOBJSM_LIFT_C_H
#include "types.h"

struct daObjSm_Lift_c {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x2];
    s32 mHorzSpeed;            /* 0x098 */
    u8  pad_09c[0x94];
    s32 unk_130;            /* 0x130 */
    u8  pad_134[0x1ec];
    s32 unk_320;            /* 0x320 */
    s32 mBasePosX;            /* 0x324 */
    s32 mBasePosY;            /* 0x328 */
    s32 mBasePosZ;            /* 0x32c */
    u8  unk_330;            /* 0x330 */
    u8  unk_331;            /* 0x331 */
    u8  pad_332[0x2];
    s32 mPathId;            /* 0x334 */
    s32 mNodeCount;            /* 0x338 */
    s32 mNodeIndex;            /* 0x33c */
    u8  pad_340[0xc];
    s32 mNodeStep;            /* 0x34c */
    u32 unk_350;            /* 0x350 */
    u8  pad_354[0x20];
    s32 unk_374;            /* 0x374 */
    u8  pad_378[0x4];
    s32 unk_37c;            /* 0x37c */
};

#endif
