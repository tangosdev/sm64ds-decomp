/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MirrorLuigi: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MIRRORLUIGI_H
#define MIRRORLUIGI_H
#include "types.h"
#include "ShadowModel.h"
#include "ModelAnim.h"
#include "Model.h"

struct MirrorLuigi {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    /* ModelAnim member. The cartridge's own ~MirrorLuigi calls _ZN9ModelAnimD1Ev at
       +0x0d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* Model member. The cartridge's own ~MirrorLuigi calls _ZN5ModelD1Ev at +0x138
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x138 */
    /* ShadowModel member. The cartridge's own ~MirrorLuigi calls _ZN11ShadowModelD1Ev
       at +0x188 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel;            /* 0x188 */
    u8  mAnimTexSeq;            /* 0x1b0 */
    u8  pad_1b1[0x7];
    s32 unk_1b8;            /* 0x1b8 */
    u8  pad_1bc[0x8];
    u8  mModelTexSeq;            /* 0x1c4 */
    u8  pad_1c5[0x7];
    s32 unk_1cc;            /* 0x1cc */
    u8  pad_1d0[0x8];
    u8  mNode;            /* 0x1d8 */
    /* trailing extent the ROM's `new MirrorLuigi` literal proves; see tools/opnew_sizes.py */
    u8 pad_1dc[0x30];
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char MirrorLuigi_size_must_be_0x20c[sizeof(struct MirrorLuigi) == 0x20c ? 1 : -1];

#endif
