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
 * dBgW_KcMbg at +0x30, the array stride is 0x200, and every platform loop's
 * strength-reduced pointers advance at exactly that stride. Matrix4x3 (0x30)
 * plus dBgW_KcMbg (0x1c8) plus Vector3s (0x6) naturally pads to 0x200.
 *
 * SM64DS RTTI names the implementation daDossy_c. The reconstructed
 * factory daDossy_c_classInit (historical alias
 * Dorrie_Spawn) constructs it for the DOSSY
 * registry profile.
 */
#ifndef DADOSSY_C_H
#define DADOSSY_C_H

#include "dActor_c.h"
#include "dExtAnmModel_c.h"
#include "dBgW_KcMbg.h"
#include "dBgCh_Actr.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "Vector3s.h"

struct daDossyCap_c;

struct DorriePlatform {
    Matrix4x3 mClsnNextMat;     /* 0x000 */
    dBgW_KcMbg mClsn;           /* 0x030 */
    /* A Vector3s, not the POD Vector3_16: UpdatePlatformMatrices copies a
       Vector3s local in, and the ROM copies member by member (ldrsh/strh)
       as it does for a class with a declared destructor. A POD copy goes
       by word. */
    Vector3s mRot; /* 0x1f8 */

    DorriePlatform();
    ~DorriePlatform();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char DorriePlatform_size_must_be_0x200[
    sizeof(DorriePlatform) == 0x200 ? 1 : -1];
#endif

struct daDossy_c : dActor_c {
    daDossyCap_c *mCap;         /* 0x0d4 */
    Vector3 mCapPos;            /* 0x0d8 */
    s16 mHeadRotX;              /* 0x0e4 */
    /* Frames before Dorrie accepts a rider again. ReleaseCapIfTouched
       sets it to 30 when a player's hit (0x400000) pops the cap;
       UpdateHeadDown counts it down and mounts the player only once
       it reaches 0. */
    u16 mRideCooldown;          /* 0x0e6 */
    u8  mHasCap;                /* 0x0e8 */
    u8  pad_0e9[0x3];
    dExtAnmModel_c mAnmModel;     /* 0x0ec */
    DorriePlatform mPlatforms[7]; /* 0x150 */
    dBgCh_Actr mWithMeshClsn;   /* 0xf50 */
    dCcAc_c mCylClsn1;          /* 0x110c */
    dCcAcPos_c mCylClsn2;       /* 0x1140 */
    Vector3 mHomePos;           /* 0x1180 */
    dActor_c *mClsnPlayer;      /* 0x118c */
    dActor_c *mRider;           /* 0x1190 */
    Vector3 mSpawnPos;          /* 0x1194 */
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

    /* Inline is load-bearing: src/game/actors/d_a_dossy.cpp defines the key
     * function InitResources, so it emits D1 then D0 with no homeless D2. */
    virtual ~daDossy_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* Returns 1 while she is off screen, which is when it disables all seven
       mPlatforms colliders; on screen it re-enables them. Behavior ignores
       the result. */
    int UpdatePlatformClsn();

    /* The three mState handlers of the table at ov065 0x0211d7fc, declared
       highest address first as the TU emits them. Each is named for the
       animation the handler that enters it installs:
         0 UpdateSwim      ASSET_DOSSY_SWIM_BCA, set by InitResources and by
                           UpdateHeadUp. Circles the ring, steering at the
                           player or along the tangent, until mCylClsn1 or a
                           landed mCylClsn2 player advances mState.
         1 UpdateHeadDown  ASSET_DOSSY_HEAD_DOWN_BCA, set by UpdateSwim.
                           Dips mPushDownHeight to 0x64000 and back, then
                           mounts the mClsnState 2 rider (head) once
                           mRideCooldown is 0, or gives up after 150 frames.
         2 UpdateHeadUp    ASSET_DOSSY_HEAD_UP_BCA, set by UpdateHeadDown.
                           Lifts the rider, restores the swim animation,
                           releases daPly_c no-control state 5 and returns
                           to slot 0. */
    void UpdateHeadUp();
    void UpdateHeadDown();
    void UpdateSwim();

    /* Walks the seven mPlatforms against the animated bones named by
       data_ov065_0211c078: each platform's mRot, mClsnNextMat and
       dBgW_KcMbg::Transform. Platform 2 is the head, so it also derives
       mHeadRotX, mHomePos and the cap's mCapPos from that bone. */
    void UpdatePlatformMatrices();
    void UpdateModelMatrix();

    /* The mesh colliders' stood-on handlers, through the two veneers below.
       InitResources wires platform 2 (gDossyHeadCollisionFile) to
       HeadStoodOnCallback and platforms 0 and 1 (body, neck) to
       BodyStoodOnCallback; each handler latches mClsnState -- 2 for the head,
       1 for the body and neck -- and mClsnPlayer when the other actor is the
       player (0xbf). UpdateHeadDown mounts only on mClsnState 2. */
    void OnHeadStoodOn(dActor_c *other);
    void OnBodyStoodOn(dActor_c *other);
    static void BodyStoodOnCallback(dBgW *collider, dActor_c *owner, dActor_c *other);
    static void HeadStoodOnCallback(dBgW *collider, dActor_c *owner, dActor_c *other);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDossy_c_size_must_be_0x11b8[
    sizeof(daDossy_c) == 0x11b8 ? 1 : -1];
#endif

#endif /* DADOSSY_C_H */
