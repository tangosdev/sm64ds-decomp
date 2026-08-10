/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Toad: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TOAD_H
#define TOAD_H
#include "types.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"

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
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN4ToadD0Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x0d4 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x108 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x108 */
    u8  mShadowModel;            /* 0x16c */
    u8  pad_16d[0x87];
    s32 unk_1f4;            /* 0x1f4 */
    u8  pad_1f8[0x8];
    /* Two ApproachLinear pairs: 0x200 chases 0x202 and 0x204 chases 0x206,
       at different rates. The targets are written from the player's
       direction each frame and the current values are what actually get
       used, which is what makes this a smoothed head-turn. */
    s16 mHeadYaw;            /* 0x200 */
    s16 mHeadYawTarget;            /* 0x202 */
    s16 mHeadPitch;            /* 0x204 */
    s16 mHeadPitchTarget;            /* 0x206 */
    u16 unk_208;            /* 0x208 */
    u8  pad_20a[0x1];
    u8  unk_20b;            /* 0x20b */
    u8  unk_20c;            /* 0x20c */
    u8  unk_20d;            /* 0x20d */
    u8  unk_20e;            /* 0x20e */
    u8  unk_20f;            /* 0x20f */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
