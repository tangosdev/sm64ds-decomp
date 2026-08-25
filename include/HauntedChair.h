/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HauntedChair: 5 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HAUNTEDCHAIR_H
#define HAUNTEDCHAIR_H
#include "types.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct HauntedChair {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12HauntedChairD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x124 */
    /* Matrix4x3: InitResources block-copies the 48-byte identity at
       IDENTITY_MATRIX4X3 over this range in one go, and 0x14c + 0x30 lands exactly
       on the dCcAcPos_c below -- the ShadowModel + shadow matrix pair this
       family uses everywhere. Was a u8 marker plus its pad. */
    Matrix4x3 mShadowMat;            /* 0x14c */
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x17c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12HauntedChairD0Ev.c] */
    dCcAcPos_c mdCcAcPos_c;            /* 0x17c */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x1bc -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12HauntedChairD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x1bc */
    s32 mState;            /* 0x378 */
    u8  pad_37c[0x4];
    s32 mHomePosX;            /* 0x380 */
    s32 mHomePosY;            /* 0x384 */
    s32 mHomePosZ;            /* 0x388 */
    s32 mClsnOffsetX;            /* 0x38c */
    s32 mClsnOffsetY;            /* 0x390 */
    s32 mClsnOffsetZ;            /* 0x394 */
    /* Trailing remainder, 0x10 bytes. Every marker is typed and the last field
       the five recovered functions touch ends at 0x398; HauntedChair_Spawn
       allocates 0x3a8. The reference does not document this class's members. */
    u8  pad_398[0x10];
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    int Render();
#endif
};

typedef char HauntedChair_size_must_be_0x3a8[sizeof(struct HauntedChair) == 0x3a8 ? 1 : -1];

#endif
