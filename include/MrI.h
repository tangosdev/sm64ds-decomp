/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MrI: 5 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MRI_H
#define MRI_H
#include "types.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "TextureSequence.h"

struct MrI {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    /* dActor_c::mPosX -- dActor_c.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0xc];
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_130 (+0x5c = speed),
       which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* TextureSequence member. The cartridge's own ~MrI calls _ZN15TextureSequenceD1Ev
       at +0x138 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base.
       The marker's pad stopped short of the object, so the member also takes over
       unk_144 (+0xc = the Animation base's speed), which the header declared
       separately inside it. */
    TextureSequence mTextureSequence;            /* 0x138 */
    /* ShadowModel member. The cartridge's own ~MrI calls _ZN11ShadowModelD1Ev at +0x14c
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel;            /* 0x14c */
    /* dCcAcPos_c member. The cartridge's own ~MrI calls _ZN10dCcAcPos_cD1Ev at +0x174
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x174 */
    u8  mShadowMat;            /* 0x1b4 */
    u8  pad_1b5[0x37];
    s32 unk_1ec;            /* 0x1ec */
    s32 mShadowRadiusScale;            /* 0x1f0 */
    u8  pad_1f4[0xc];
    s32 mShadowHeight;            /* 0x200 */
    u8  pad_204[0x8];
    s16 mTurnRefAngleY;            /* 0x20c */
    u8  pad_20e[0x8];
    u8  mTimer;            /* 0x216 */
    u8  unk_217;            /* 0x217 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char MrI_size_must_be_0x218[sizeof(struct MrI) == 0x218 ? 1 : -1];

#endif
