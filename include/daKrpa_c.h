#ifndef FLAMECHOMP_H
#define FLAMECHOMP_H

#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"
#include "dActor_c.h"

/* FlameChomp is daKrpa_c in the ROM's own RTTI: the typeinfo at ov070
 * 0x02123340 names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x02123370 (31 slots, same count as dActor_c's) is what pairs it
 * to FlameChomp_Spawn, which stores that address after allocating 0x3b0 bytes
 * via fBase_c::operator new.
 *
 * The Spawn constructs the four owned subobjects below at 0xd4..0x1a0 in
 * declaration order; D1 destroys them in exactly the reverse order before
 * chaining to dActor_c::~dActor_c -- two independent witnesses for each
 * member's type and offset.
 *
 * Own vtable slots, from the ROM table diffed against dActor_c's: 0
 * InitResources, 3 CleanupResources, 6 Behavior, 9 Render, 12
 * OnPendingDestroy, 16/17 the destructor pair, 18 OnYoshiTryEat. Every other
 * slot is inherited.
 *
 * The header this replaces was deliberately flat -- a non-deriving struct
 * whose leading "fields" duplicated dActor_c's storage -- because giving a
 * non-derived struct a virtual would have inserted a vptr and shifted every
 * offset. Deriving from dActor_c is what makes the declarations below honest.
 */
struct FlameChomp : dActor_c {
    u8                        pad_0d0[0x4];
    ModelAnim                 mModelAnim;                    /* 0x0d4 */
    ShadowModel               mShadowModel;                  /* 0x138 */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;    /* 0x160 */
    WithMeshClsn              mWithMeshClsn;                 /* 0x1a0 */
    u8                        unk_35c;                       /* 0x35c */
    u8                        pad_35d[0x4b];
    s32                       unk_3a8;                       /* 0x3a8 */
    u8                        pad_3ac[0x4];

    /* Declared first on purpose, same reasoning as dActor_c.h: the key
       function pins where mwcc anchors the vtable. */
    virtual ~FlameChomp();

    virtual s32  InitResources();       /* slot 0 */
    virtual s32  CleanupResources();    /* slot 3 */
    virtual s32  Behavior();            /* slot 6 */
    virtual s32  Render();              /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 */
};

typedef char FlameChomp_size_must_be_0x3b0[
    sizeof(FlameChomp) == 0x3b0 ? 1 : -1];

#endif /* FLAMECHOMP_H */
