#ifndef FLAMECHOMPFIRE_H
#define FLAMECHOMPFIRE_H

#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"
#include "dActor_c.h"

/* FlameChompFire is daKpFr_c in the ROM's own RTTI: the typeinfo at ov070
 * 0x02123418 names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x02123448 (31 slots, same count as dActor_c's) is what pairs it
 * to FlameChompFire_Spawn, which stores that address after allocating 0x330
 * bytes via fBase_c::operator new.
 *
 * The Spawn constructs the three owned subobjects below at 0xd4..0x130 in
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
struct FlameChompFire : dActor_c {
    u8                 pad_0d0[0x4];
    ShadowModel        mShadowModel;           /* 0x0d4 */
    MovingCylinderClsn mMovingCylinderClsn;    /* 0x0fc */
    WithMeshClsn       mWithMeshClsn;          /* 0x130 */
    u8                 unk_2ec;                /* 0x2ec */
    u8                 pad_2ed[0x37];
    s32                unk_324;                /* 0x324 */
    s32                unk_328;                /* 0x328 */
    u8                 pad_32c[0x4];

    /* Declared first on purpose, same reasoning as dActor_c.h: the key
       function pins where mwcc anchors the vtable. */
    virtual ~FlameChompFire();

    virtual s32  InitResources();       /* slot 0 */
    virtual s32  CleanupResources();    /* slot 3 */
    virtual s32  Behavior();            /* slot 6 */
    virtual s32  Render();              /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 */
};

typedef char FlameChompFire_size_must_be_0x330[
    sizeof(FlameChompFire) == 0x330 ? 1 : -1];

#endif /* FLAMECHOMPFIRE_H */
