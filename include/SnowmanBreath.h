#ifndef SNOWMANBREATH_H
#define SNOWMANBREATH_H

#include "dActor_c.h"
#include "dCcPos_c.h"
#include "math/Matrix.h"

struct Player;

/* One collision-bearing breath particle. The constructor/destructor calls in
 * the ROM prove composition, not inheritance: dCcPos_c lives at offset zero,
 * and the array helpers use a stride of 0x60. The descriptive class and method
 * names are inferred from the call graph; their ownership and layouts are not.
 */
struct SnowmanBreathParticle {
    dCcPos_c mCollider;         /* 0x00 */
    u32 mParticleID;            /* 0x3c */
    Vector3 mPos;               /* 0x40 */
    u8 mPad4c[0xc];             /* 0x4c */
    s16 mAngleX;                /* 0x58 */
    s16 mAngleY;                /* 0x5a */
    s16 mAngleZ;                /* 0x5c */
    u8 mTimer;                  /* 0x5e */
    u8 mPad5f;                  /* 0x5f */

    SnowmanBreathParticle();
    ~SnowmanBreathParticle();

    void HitPlayer();
    void CheckSnowman();
    void UpdatePosition();
    void Render();
    void Behavior();
    int TrySpawn(Player &player);
};

typedef char SnowmanBreathParticle_size_must_be_0x60[
    sizeof(SnowmanBreathParticle) == 0x60 ? 1 : -1];

/* The cartridge RTTI calls this class daSnmBth_c. SnowmanBreath is the
 * readable compatibility spelling already carried by all seven named
 * virtuals. Its __si_class_type_info record names dActor_c as its sole base at
 * offset zero, and its vtable has exactly the same 31 slots as dActor_c.
 *
 * The deleting/complete destructors and factory independently pin the base,
 * the 50-element particle array, and the total 0x13d4 allocation. The matrix
 * at 0x1394 is copied as 0x30 bytes and later used as the inverse transform for
 * the player's local-range test.
 */
struct SnowmanBreath : dActor_c {
    u8 mPad0d0[0x4];                       /* 0x0d0 */
    SnowmanBreathParticle mParticles[50];  /* 0x0d4 */
    Matrix4x3 mInvModelMat;                /* 0x1394 */
    Player *mTalkPlayer;                   /* 0x13c4 */
    s32 mTalkTimer;                        /* 0x13c8 */
    u32 mSoundHandle;                      /* 0x13cc */
    u8 mTalkDone;                          /* 0x13d0 */
    u8 mPad13d1;                           /* 0x13d1 */
    u8 mTalkState;                         /* 0x13d2 */
    u8 mNextParticle;                      /* 0x13d3 */

    virtual ~SnowmanBreath();              /* slots 16, 17 */

    virtual int InitResources();           /* slot  0 */
    virtual int CleanupResources();        /* slot  3 */
    virtual int Behavior();                /* slot  6 */
    virtual int Render();                  /* slot  9 */
    virtual void OnPendingDestroy();       /* slot 12 */

    int IsPlayerInRange();
};

typedef char SnowmanBreath_size_must_be_0x13d4[
    sizeof(SnowmanBreath) == 0x13d4 ? 1 : -1];

#endif
