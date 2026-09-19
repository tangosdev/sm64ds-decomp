/* daDossy_c, reconstructed from its factory, destructor pair, vtable/RTTI and
 * five matched methods.
 *
 * NAME: daDossy_c is the cartridge's own RTTI spelling. _ZTS at ov065
 * 0x0211cd34 is the length-prefixed byte string "9daDossy_c"
 * (39 64 61 44 6f 73 73 79 5f 63 00), and the type-info word four bytes below
 * the vtable address point at 0x0211ce48 reads _ZTI9daDossy_c (0x0211cd40), so
 * the vtable this tree used to spell _ZTV6Dorrie is this type's. Dorrie was a
 * coined name; the ROM-owned vtable still has to be checked directly for its
 * exact 31-slot dActor_c shape.
 *
 * DorriePlatform is independently corroborated by the two 0x1c callbacks used
 * by daDossy_c_classInit and the D1/D0 pair: each callback constructs or destroys a
 * dBgW_KcMbg at +0x30, the array stride is 0x200, and InitResources advances
 * the matrix and collider views at exactly that stride. Matrix4x3 (0x30) plus
 * dBgW_KcMbg (0x1c8) plus Vector3_16 (0x6) naturally pads to 0x200.
 *
 * SM64DS RTTI names the implementation daDossy_c. The reconstructed
 * factory daDossy_c_classInit (historical alias
 * Dorrie_Spawn) constructs it for the DOSSY
 * registry profile.
 */
#ifndef DADOSSY_C_H
#define DADOSSY_C_H

#include "dActor_c.h"
#include "ModelAnim.h"
#include "dBgW_KcMbg.h"
#include "dBgCh_Actr.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"

struct daDossyCap_c;

struct DorriePlatform {
    Matrix4x3 mClsnNextMat;     /* 0x000 */
    dBgW_KcMbg mClsn;           /* 0x030 */
    Vector3_16 mRot;            /* 0x1f8 */

    DorriePlatform();
    ~DorriePlatform();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char DorriePlatform_size_must_be_0x200[
    sizeof(DorriePlatform) == 0x200 ? 1 : -1];
#endif

struct daDossy_c : dActor_c {
    u32 unk_0d0;                 /* 0x0d0 */
    daDossyCap_c *mCap;         /* 0x0d4 */
    s32 mCapPosX;               /* 0x0d8 */
    s32 mCapPosY;               /* 0x0dc */
    s32 mCapPosZ;               /* 0x0e0 */
    s16 mHeadRotX;              /* 0x0e4 */
    u16 mUnkTimer;              /* 0x0e6 */
    u8  mHasCap;                /* 0x0e8 */
    u8  pad_0e9[0x3];
    ModelAnim mModelAnim;       /* 0x0ec */
    DorriePlatform mPlatforms[7]; /* 0x150 */
    dBgCh_Actr mWithMeshClsn;   /* 0xf50 */
    dCcAc_c mCylClsn1;          /* 0x110c */
    dCcAcPos_c mCylClsn2;       /* 0x1140 */
    s32 mHomePosX;              /* 0x1180 */
    s32 mHomePosY;              /* 0x1184 */
    s32 mHomePosZ;              /* 0x1188 */
    dActor_c *mClsnPlayer;      /* 0x118c */
    dActor_c *mRider;           /* 0x1190 */
    s32 mSpawnPosX;             /* 0x1194 */
    s32 mSpawnPosY;             /* 0x1198 */
    s32 mSpawnPosZ;             /* 0x119c */
    s32 mDistToCenter;          /* 0x11a0 */
    s16 mAngToCenter;           /* 0x11a4 */
    s16 mAngVelY;               /* 0x11a6 */
    s32 mPushDownHeight;        /* 0x11a8 */
    s32 mSinkHeight;            /* 0x11ac */
    u8  mStateState;            /* 0x11b0 */
    u8  pad_11b1;
    u16 mStateTimer;            /* 0x11b2 */
    u8  mState;                 /* 0x11b4 */
    u8  mClsnState;             /* 0x11b5 */
    u8  pad_11b6[0x2];

    /* Inline is load-bearing: explicit use in the destructor source files
     * emits D1 then D0 without inventing a homeless D2. */
    virtual ~daDossy_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDossy_c_size_must_be_0x11b8[
    sizeof(daDossy_c) == 0x11b8 ? 1 : -1];
#endif

#endif /* DADOSSY_C_H */
