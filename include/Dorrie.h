/* Dorrie, reconstructed from its factory, destructor pair, vtable/RTTI and
 * five matched methods.
 *
 * The cartridge calls this class daDossy_c in RTTI, while its configured
 * methods use the compatibility spelling Dorrie. The compiler-facing Dorrie
 * metadata is therefore a per-function passenger; the ROM-owned vtable still
 * has to be checked directly for its exact 31-slot dActor_c shape.
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
#ifndef DORRIE_H
#define DORRIE_H

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

typedef char DorriePlatform_size_must_be_0x200[
    sizeof(DorriePlatform) == 0x200 ? 1 : -1];

struct Dorrie : dActor_c {
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
    virtual ~Dorrie() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

typedef char Dorrie_size_must_be_0x11b8[
    sizeof(Dorrie) == 0x11b8 ? 1 : -1];

#endif /* DORRIE_H */
