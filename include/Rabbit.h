/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Rabbit: 5 matched functions, 27 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef RABBIT_H
#define RABBIT_H
#include "types.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

struct Rabbit {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* Actor::mCamSpacePos. Behavior hands its address to the sound helper
       func_02012694, which is the same call shape Clam and Toad make. */
    s32 mCamSpacePosX;            /* 0x074 */
    u8  pad_078[0x8];
    s32 mScale;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    /* Behavior copies 0x92/0x94/0x96 into 0x8c/0x8e/0x90 in one go, which is
       the Actor angle / previous-angle pair -- same shape WaterRing::Behavior
       uses. Names taken from Actor.h rather than invented. */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    s32 mHorzSpeed;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  unk_0cc;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 unk_0d0;            /* 0x0d0 */
    u8  pad_0d4[0x2c];
    u16 unk_100;            /* 0x100 -- DecIfAbove0_Short ticks it every frame */
    u8  pad_102[0x2];
    u16 unk_104;            /* 0x104 */
    u8  pad_106[0x1];
    u8  unk_107;            /* 0x107 */
    u8  pad_108[0x8];
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6RabbitD1Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x110 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x144 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6RabbitD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x144 */
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x7];
    u8  unk_308;            /* 0x308 */
    u8  pad_309[0x3];
    u8  unk_30c;            /* 0x30c */
    u8  pad_30d[0x4f];
    s32 unk_35c;            /* 0x35c */
    u8  pad_360[0x4];
    /* The current animation descriptor. Behavior compares it against four
       ov085 data objects to decide what state it is in, so it is a pointer
       even though nothing here dereferences it as one type. */
    s32 unk_364;            /* 0x364 */
    u8  mShadowModel1;            /* 0x368 */
    u8  pad_369[0x57];
    u8  mShadowModel2;            /* 0x3c0 */
    u8  pad_3c1[0x5f];
    s32 mColorVariant;            /* 0x420 */
    u8  pad_424[0x2];
    u8  unk_426;            /* 0x426 */
    u8  unk_427;            /* 0x427 -- talk step: 0 idle, 1 talking, 2 done */
    u8  unk_428;            /* 0x428 */
    u8  unk_429;            /* 0x429 */
    u8  unk_42a;            /* 0x42a -- counts to 0x96 then clears unk_107 */
    u8  pad_42b[0xd];
    s32 unk_438;            /* 0x438 */
    s32 mRabbitId;            /* 0x43c */
    s32 mCharacterId;            /* 0x440 */
    s32 unk_444;            /* 0x444 */
    s32 unk_448;            /* 0x448 */
    u8  pad_44c[0x10];
    s32 unk_45c;            /* 0x45c */
    u8  pad_460[0x8];
    s32 unk_468;            /* 0x468 */
    u8  pad_46c[0x4];
    /* Particle::System::New's handle, passed back in as its first argument
       each frame -- so the same emitter is updated rather than respawned. */
    s32 unk_470;            /* 0x470 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    void OnPendingDestroy();
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
