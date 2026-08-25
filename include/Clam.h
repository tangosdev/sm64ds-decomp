/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Clam: 5 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLAM_H
#define CLAM_H
#include "types.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

struct Clam {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* dActor_c::mCamSpacePos -- dActor_c.h declares s32 here. The sound call in
       Behavior takes its address, which is what pins it. */
    s32 mCamSpacePosX;            /* 0x074 */
    u8  pad_078[0x5c];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x138 */
    /* 0 shut, 1 lunging -- the only two values Behavior's switch has arms
       for, and InitResources sets it to 0. */
    u8  mState;            /* 0x16c */
    u8  pad_16d[0x1];
    u16 mStateTimer;            /* 0x16e */
    u16 mShutTimer;            /* 0x170 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char Clam_size_must_be_0x174[sizeof(struct Clam) == 0x174 ? 1 : -1];

#endif
