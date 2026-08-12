#ifndef DAOBJMARIOCAP_C_H
#define DAOBJMARIOCAP_C_H

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
 *   _ZTV15daObjMarioCap_c, then constructs MovingCylinderClsn 0x110, WithMeshClsn 0x144,
 *   ModelAnim 0x300, ShadowModel 0x364 and the CapIcon at 0x3d0.
 *
 *   _ZN15daObjMarioCap_cD1Ev tears the same five down in exactly the reverse order and
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
 * THE CLASS USED TO BE CALLED WaterfallMist, and the previous revision of this comment
 * said the name was "probably wrong" but left it. It is wrong, and the RTTI settles it
 * outright rather than by inference: build/rtti.json has a record at ov002 0x021095ac,
 * mangled 15daObjMarioCap_c, whose `vtable` field is 0x021095f0 -- the very address the
 * tree was calling _ZTV13WaterfallMist. The circumstantial evidence all points the same
 * way: the factory is Cap_Spawn, and the class holds a CapIcon.
 *
 * WaterfallMist_Spawn and WaterfallMist_SpawnInfo are NOT renamed, and that is not an
 * oversight. They belong to a different actor: WaterfallMist_Spawn allocates 220 bytes
 * and stores the vtable at 0x021094a0, whose RTTI record is daObjWaterfall_c. For that
 * class the name is apt, so it stays. The defect was one name serving two classes.
 *
 * A SEPARATE DEFECT FOUND ON THE WAY, recorded here and not fixed: _ZTV13WaterfallMist
 * is attached to 0x021094a0, which is daObjWaterfall_c's table. WaterfallMist' own
 * vtable is 0x021093e0 (RTTI: daObjLava_c) and carries no _ZTV name at all --
 * PoppingLavaBubbles_Spawn stores it as the unnamed _ZTV18PoppingLavaBubbles. So that symbol
 * names the wrong table, one entry along.
 */
struct daObjMarioCap_c : Enemy {
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

    virtual ~daObjMarioCap_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
};

typedef char daObjMarioCap_c_size_must_be_0x410[sizeof(daObjMarioCap_c) == 0x410 ? 1 : -1];

#endif /* DAOBJMARIOCAP_C_H */
