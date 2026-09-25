/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class daEykn_c: 5 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DAEYKN_C_H
#define DAEYKN_C_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "TextureSequence.h"

/* The ROM's RTTI name for this class is daEykn_c: _ZTS8daEykn_c at ov071
 * 0x02122cd8, and _ZTI8daEykn_c at 0x02122ce4 is an __si_class_type_info whose
 * base is _ZTI8dActor_c, so the base is dActor_c. The vtable is _ZTV8daEykn_c
 * (address point 0x02122d30); the tree's old coined name MrI only aliased
 * _ZTV3MrI to that same address, and the alias is gone. */
struct daEykn_c : dActor_c {
    /* dActor_c ends at 0x0d0; the derived payload begins at 0x0d4. */
    u8 pad_0d0[0x4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_130 (+0x5c = speed),
       which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* TextureSequence member. The cartridge's own ~daEykn_c calls _ZN15TextureSequenceD1Ev
       at +0x138 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base.
       The marker's pad stopped short of the object, so the member also takes over
       unk_144 (+0xc = the Animation base's speed), which the header declared
       separately inside it. */
    TextureSequence mTextureSequence;            /* 0x138 */
    /* ShadowModel member. The cartridge's own ~daEykn_c calls _ZN11ShadowModelD1Ev at +0x14c
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel;            /* 0x14c */
    /* dCcAcPos_c member. The cartridge's own ~daEykn_c calls _ZN10dCcAcPos_cD1Ev at +0x174
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
    virtual ~daEykn_c();                          /* slots 16/17 */
    virtual s32 InitResources();             /* slot 0 */
    virtual s32 CleanupResources();          /* slot 3 */
    virtual s32 Behavior();                  /* slot 6 */
    virtual s32 Render();                    /* slot 9 */
    virtual void OnPendingDestroy();         /* slot 12 -- empty body in the ROM */
#endif
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daEykn_c_size_must_be_0x218[sizeof(struct daEykn_c) == 0x218 ? 1 : -1];
#endif

#endif
