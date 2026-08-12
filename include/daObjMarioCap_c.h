#ifndef WATERFALLMIST_H
#define WATERFALLMIST_H

#include "types.h"
#include "Enemy.h"
#include "CapIcon.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

/* Derives from Enemy, and both witnesses agree offset for offset:
 *
 *   Cap_Spawn (ov002) allocates 0x410, calls _ZN5EnemyC2Ev, stores
 *   _ZTV13WaterfallMist, then constructs MovingCylinderClsn 0x110, WithMeshClsn 0x144,
 *   ModelAnim 0x300, ShadowModel 0x364 and the CapIcon at 0x3d0.
 *
 *   _ZN13WaterfallMistD1Ev tears the same five down in exactly the reverse order and
 *   chains to _ZN5EnemyD2Ev.
 *
 * THE 0x3d0 MEMBER IS A CapIcon, and that is what this class was waiting on. Its
 * constructor and destructor are func_ov001_020ab3c4 / func_ov001_020ab3a0 -- the same
 * pair CapEnemy holds at its own 0x164, typed in include/CapIcon.h. Left as padding
 * the destructor emits a short chain and comes out a different SIZE, which reads as
 * `999 word(s) differ` and looks like a total failure rather than one missing member.
 *
 * SIZE 0x410, the literal in Cap_Spawn's ActorBase::operator new. CapIcon is 0x1c, so
 * 0x3d0 + 0x1c = 0x3ec closes onto the scalars below it.
 *
 * THE NAME IS PROBABLY WRONG, and this is the place to say so rather than quietly fix
 * it. The ROM's RTTI calls this class daObjMarioCap_c; its factory is Cap_Spawn, not
 * WaterfallMist_Spawn (that one allocates 220 bytes and stores
 * _ZTV18PoppingLavaBubbles, so it belongs to a different class entirely); and it holds
 * a CapIcon. Every piece of evidence says "Mario's cap", not "waterfall mist".
 * Renaming it would move nine functions and the vtable symbol, so it is left as a
 * separate, deliberate change rather than smuggled in here.
 */
struct WaterfallMist : Enemy {
    MovingCylinderClsn  mMovingCylinderClsn;    /* 0x110 */
    WithMeshClsn        mWithMeshClsn;          /* 0x144 */
    ModelAnim           mModelAnim;             /* 0x300 */
    ShadowModel         mShadowModel;           /* 0x364 */
    u8  pad_38c[0x30];
    s32 unk_3bc;                                /* 0x3bc */
    s32 unk_3c0;                                /* 0x3c0 */
    s32 unk_3c4;                                /* 0x3c4 */
    s32 unk_3c8;                                /* 0x3c8 */
    s32 unk_3cc;                                /* 0x3cc */
    CapIcon mCapIcon;                           /* 0x3d0 */
    s32 unk_3ec;                                /* 0x3ec */
    s32 mType;                                  /* 0x3f0 */
    s32 mModelIndex;                            /* 0x3f4 */
    u8  pad_3f8[0x7];
    u8  unk_3ff;                                /* 0x3ff */
    u8  unk_400;                                /* 0x400 */
    u8  unk_401;                                /* 0x401 */
    u8  pad_402[0xe];

    virtual ~WaterfallMist();

    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
};

typedef char WaterfallMist_size_must_be_0x410[sizeof(WaterfallMist) == 0x410 ? 1 : -1];

#endif /* WATERFALLMIST_H */
