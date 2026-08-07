/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Klepto: 5 matched functions, 42 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KLEPTO_H
#define KLEPTO_H
#include "types.h"
#include "BlendModelAnim.h"

struct Klepto {
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
    u8  pad_098[0x4];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x33];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsn1;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mMovingCylinderClsn2;            /* 0x144 */
    u8  pad_145[0x33];
    u8  mWithMeshClsn;            /* 0x178 */
    u8  pad_179[0x1bb];
    /* BlendModelAnim member, named by _ZN14BlendModelAnimD1Ev at +0x334 -- a relocation
       the ROM build checks. D1 and not D2, so it is this type and not an inlined base. The
       marker's pad stopped short of the object, so the member also takes over unk_390
       (+0x5c = speed), which the header declared separately inside it. */
    BlendModelAnim mBlendModelAnim;            /* 0x334 */
    u8  mShadowModel;            /* 0x3a4 */
    u8  pad_3a5[0x87];
    s32 unk_42c;            /* 0x42c */
    s32 unk_430;            /* 0x430 */
    s32 unk_434;            /* 0x434 */
    s32 unk_438;            /* 0x438 */
    u8  pad_43c[0x8];
    s16 unk_444;            /* 0x444 */
    u8  pad_446[0x2];
    u8  unk_448;            /* 0x448 */
    u8  pad_449[0x1];
    s16 unk_44a;            /* 0x44a */
    s32 mHeldActorID;            /* 0x44c */
    s32 unk_450;            /* 0x450 */
    s32 unk_454;            /* 0x454 */
    s32 unk_458;            /* 0x458 */
    u8  pad_45c[0x8];
    s32 mPathId;            /* 0x464 */
    s32 mCarriedItem;            /* 0x468 */
    s32 unk_46c;            /* 0x46c */
    s32 unk_470;            /* 0x470 */
    u32 unk_474;            /* 0x474 */
    u8  pad_478[0xc];
    s32 unk_484;            /* 0x484 */
    s32 unk_488;            /* 0x488 */
    s32 unk_48c;            /* 0x48c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
