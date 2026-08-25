/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Toad: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TOAD_H
#define TOAD_H
#include "types.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"

struct Toad {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    /* Behavior turns toward the player with ApproachLinear on this and
       measures AngleDiff against it, which is what pins it as the yaw. */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN4ToadD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x108 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x108 */
    /* ShadowModel member. The cartridge's own ~Toad calls _ZN11ShadowModelD1Ev at
       +0x16c (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel;            /* 0x16c */
    u8  pad_194[0x60];
    s32 mCapUniqueID;            /* 0x1f4 */
    u8  pad_1f8[0x8];
    /* Two ApproachLinear pairs: 0x200 chases 0x202 and 0x204 chases 0x206,
       at different rates. The targets are written from the player's
       direction each frame and the current values are what actually get
       used, which is what makes this a smoothed head-turn. */
    s16 mHeadYaw;            /* 0x200 */
    s16 mHeadYawTarget;            /* 0x202 */
    s16 mHeadPitch;            /* 0x204 */
    s16 mHeadPitchTarget;            /* 0x206 */
    u16 mMessageID;            /* 0x208 */
    u8  pad_20a[0x1];
    u8  mVariant;            /* 0x20b */
    u8  mStarID;            /* 0x20c */
    u8  mOpacity;            /* 0x20d */
    u8  mTargetOpacity;            /* 0x20e */
    u8  mStarReqIndex;            /* 0x20f */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

typedef char Toad_size_must_be_0x210[sizeof(struct Toad) == 0x210 ? 1 : -1];

#endif
