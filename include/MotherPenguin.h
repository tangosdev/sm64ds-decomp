/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MotherPenguin: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MOTHERPENGUIN_H
#define MOTHERPENGUIN_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct MotherPenguin {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_0dc (+0x8 = data),
       mAnimation (+0x50 = the Animation base), which the header declared separately inside
       it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* TextureSequence member, named by the class's own destructor calling
       TextureSequence's D1 at +0x138 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MotherPenguinD0Ev.c] */
    TextureSequence mTextureSequence;            /* 0x138 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x14c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MotherPenguinD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x14c */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x174 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MotherPenguinD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x174 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x1a8 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MotherPenguinD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x1a8 */
    s32 unk_364;            /* 0x364 */
    s32 unk_368;            /* 0x368 */
    s32 unk_36c;            /* 0x36c */
    u8  pad_370[0x4];
    s32 unk_374;            /* 0x374 */
    /* Trailing remainder, 0x14 bytes. Every marker is already typed and the
       last field the five recovered functions touch ends at 0x378;
       MotherPenguin_Spawn allocates 0x38c. The reference does not document
       this class's members. */
    u8  pad_378[0x14];
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char MotherPenguin_size_must_be_0x38c[sizeof(struct MotherPenguin) == 0x38c ? 1 : -1];

#endif
