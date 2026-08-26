/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MrI: 5 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MRI_H
#define MRI_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "TextureSequence.h"

/* The ROM's RTTI name for this class is daEykn_c (_ZTI8daEykn_c), while the
 * established function-symbol view spells its methods as MrI.  MrI is the
 * compiler-facing compatibility name; config/arm9/overlays/ov071/symbols.txt
 * deliberately aliases its vtable view (_ZTV3MrI) to the cartridge's
 * _ZTV8daEykn_c address. */
struct MrI : dActor_c {
    /* dActor_c ends at 0x0d0; the derived payload begins at 0x0d4. */
    u8 pad_0d0[0x4];
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
    virtual ~MrI();                          /* slots 16/17 */
    virtual s32 InitResources();             /* slot 0 */
    virtual s32 CleanupResources();          /* slot 3 */
    virtual s32 Behavior();                  /* slot 6 */
    virtual s32 Render();                    /* slot 9 */
    virtual void OnPendingDestroy();         /* slot 12 -- empty body in the ROM */
#endif
};

typedef char MrI_size_must_be_0x218[sizeof(struct MrI) == 0x218 ? 1 : -1];

#endif
