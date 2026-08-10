#ifndef SWOOP_H
#define SWOOP_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN5SwoopD1Ev`
 * stores this vtable, destroys five members, then calls `Enemy::~Enemy`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The five members close exactly on one another, and Enemy's own 0x110 closes
 * exactly on the first. The destructor destroys TWO ModelAnims, which is what
 * makes the pair at 0x300 and 0x364 two members rather than one and a gap:
 *
 *     0x110 MovingCylinderClsn  0x34   -> 0x144
 *     0x144 WithMeshClsn        0x1bc  -> 0x300
 *     0x300 ModelAnim           0x64   -> 0x364
 *     0x364 ModelAnim           0x64   -> 0x3c8
 *     0x3c8 ShadowModel         0x28   -> 0x3f0
 *
 * Typing them absorbed unk_35c and unk_3c0, each at +0x5c inside its own
 * ModelAnim -- the `speed` of that Animation base (base at +0x50, speed at
 * +0x0c). Two markers, one explanation, which is why the pair corroborates
 * rather than merely fits.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct Swoop : Enemy {
    /* What mCurrentState points at. Behavior compares it against objects in
       ov065's data and calls the handler at +0x08 through it. Only that
       handler is evidenced. */
    struct State {
        u8  pad_00[0x8];
        void (Swoop::*mMain)();       /* 0x08 */
    };

    MovingCylinderClsn mMovingCylinderClsn; /* 0x110 */
    WithMeshClsn mWithMeshClsn;       /* 0x144 */
    ModelAnim mModelAnim1;            /* 0x300 */
    ModelAnim mModelAnim2;            /* 0x364 */
    ShadowModel mShadowModel;         /* 0x3c8 */
    u8  pad_3f0[0x30];
    /* The state pointer was reachable only as raw `c + 0x420`, so the
       auto-generated header never listed it. */
    State *mCurrentState;             /* 0x420 */
    /* InitResources copies the spawn position into these three. */
    s32 mHomePosX;                    /* 0x424 */
    s32 mHomePosY;                    /* 0x428 */
    s32 mHomePosZ;                    /* 0x42c */
    u8  pad_430[0xc];
    u8  unk_43c;                      /* 0x43c */

    /* --- vtable --- */
    virtual ~Swoop();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Swoop_size_must_be_0x440[sizeof(Swoop) == 0x440 ? 1 : -1];

#endif /* SWOOP_H */
