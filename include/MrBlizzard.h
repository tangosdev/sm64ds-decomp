#ifndef MRBLIZZARD_H
#define MRBLIZZARD_H

#include "types.h"

/* Derives from Enemy: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct MrBlizzard : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;/* 0x110 */
    WithMeshClsn mWithMeshClsn;       /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    ShadowModel mShadowModel;         /* 0x370 */
    u8  pad_398[0x64];
    s32 unk_3fc;                      /* 0x3fc */
    s32 unk_400;                      /* 0x400 */
    u8  pad_404[0x10];
    s16 unk_414;                      /* 0x414 */
    u8  pad_416[0x2];
    s32 mPathId;                      /* 0x418 */
    s32 mType;                        /* 0x41c */
    s32 unk_420;                      /* 0x420 */
    s32 unk_424;                      /* 0x424 */
    u8  pad_428[0x24];
    s32 unk_44c;                      /* 0x44c */
    s32 unk_450;                      /* 0x450 */
    s32 unk_454;                      /* 0x454 */

    /* --- vtable --- */
    virtual ~MrBlizzard();

    int Behavior();
    int InitResources();
    int Render();
};

typedef char MrBlizzard_size_must_be_0x458[sizeof(MrBlizzard) == 0x458 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct MrBlizzard {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3b];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x5];
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10MrBlizzardD1Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x110 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10MrBlizzardD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x150 */
    /* ModelAnim member, named by the class's own destructor calling
       ModelAnim's D1 at +0x30c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10MrBlizzardD1Ev.c] */
    ModelAnim mModelAnim;            /* 0x30c */
    u8  mShadowModel;            /* 0x370 */
    u8  pad_371[0x8b];
    s32 unk_3fc;            /* 0x3fc */
    s32 unk_400;            /* 0x400 */
    u8  pad_404[0x10];
    s16 unk_414;            /* 0x414 */
    u8  pad_416[0x2];
    s32 mPathId;            /* 0x418 */
    s32 mType;            /* 0x41c */
    s32 unk_420;            /* 0x420 */
    s32 unk_424;            /* 0x424 */
    u8  pad_428[0x24];
    s32 unk_44c;            /* 0x44c */
    s32 unk_450;            /* 0x450 */
    s32 unk_454;            /* 0x454 */
};

#endif /* __cplusplus */

#endif /* MRBLIZZARD_H */
