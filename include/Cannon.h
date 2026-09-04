#ifndef CANNON_H
#define CANNON_H

#include "types.h"
#include "Model.h"
#include "dCcAc_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

/* The ROM constructs dActor_c first, then the owned Model and dCcAc_c in
 * daCnn_c_classInit. D1/D0 destroy those members in reverse before the actor base;
 * together the two directions pin this inheritance and layout. */
struct Cannon : dActor_c {
    u8 pad_0d0[0x4];         /* 0x0d0 */
    Model mModel;             /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x124 */
    u8 pad_158[0x4];
    s32 mSpawnPosX;           /* 0x15c */
    s32 mSpawnPosY;           /* 0x160 */
    s32 mSpawnPosZ;           /* 0x164 */
    u8 pad_168[0xc];
    s32 unk_174;              /* 0x174 */
    s16 mAimTimer;            /* 0x178 */
    s16 mTargetAngleY;        /* 0x17a */
    s16 mHomeAngleY;          /* 0x17c */
    u8 pad_17e[0x2];
    s32 mState;               /* 0x180 */
    u8 mVariant;              /* 0x184 */
    u8 mFireStep;             /* 0x185 */
    u8 pad_186[0xe];
    s32 unk_194;              /* 0x194 */

    virtual ~Cannon();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#else

/* Flat layout for the remaining C translation units. */
struct Cannon {
    u8  pad_000[0xd4];
    Model mModel;            /* 0x0d4 */
    dCcAc_c mdCcAc_c;            /* 0x124 */
    u8  pad_158[0x4];
    s32 mSpawnPosX;            /* 0x15c */
    s32 mSpawnPosY;            /* 0x160 */
    s32 mSpawnPosZ;            /* 0x164 */
    u8  pad_168[0xc];
    s32 unk_174;            /* 0x174 */
    s16 mAimTimer;            /* 0x178 */
    s16 mTargetAngleY;            /* 0x17a */
    s16 mHomeAngleY;            /* 0x17c */
    u8  pad_17e[0x2];
    s32 mState;            /* 0x180 */
    u8  mVariant;            /* 0x184 */
    u8  mFireStep;            /* 0x185 */
    u8  pad_186[0xe];
    s32 unk_194;            /* 0x194 */
};

#endif

typedef char Cannon_size_must_be_0x198[sizeof(struct Cannon) == 0x198 ? 1 : -1];

#endif /* CANNON_H */
