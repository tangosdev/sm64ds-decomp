#ifndef DAEYBM_C_H
#define DAEYBM_C_H

#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"
#include "dActor_c.h"
#include "math/Matrix.h"

/* daEyBm_c is the ROM's own RTTI name for this class (this tree once coined it
 * MrI_Projectile): the typeinfo at ov071
 * 0x02122db8 names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x02122de8 (31 slots, same count as dActor_c's) is what pairs it
 * to MrI_Projectile_Spawn, which stores that address after allocating 0x334
 * bytes via fBase_c::operator new.
 *
 * The Spawn constructs the three owned subobjects below at 0xd4..0x13c in
 * declaration order; D1 destroys them in exactly the reverse order before
 * chaining to dActor_c::~dActor_c -- two independent witnesses for each
 * member's type and offset. The flat header this replaces carried all three
 * as single u8 markers.
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
struct daEyBm_c : dActor_c {
    u8                        pad_0d0[0x4];
    ShadowModel               mShadowModel;                  /* 0x0d4 */
    dCcAcPos_c mdCcAcPos_c;    /* 0x0fc */
    dBgCh_Actr              mWithMeshClsn;                 /* 0x13c */
    Matrix4x3 mMatrix;        /* 0x2f8 */
    /* Two particle handles, effects 0x46 and 0x47, both fed back into
       Particle::System::NewUnkCallback818 every Render. mLifeTimer starts at
       0x96 (150 frames) and is counted down by Behavior.
       [_ZN8daEyBm_c6RenderEv.cpp, _ZN8daEyBm_c8BehaviorEv.cpp,
        _ZN8daEyBm_c13InitResourcesEv.cpp] */
    u32                       mParticle1;                       /* 0x328 */
    u32                       mParticle2;                       /* 0x32c */
    u16                       mLifeTimer;                       /* 0x330 */
    u8                        pad_332[0x2];

    /* Declared first on purpose, same reasoning as dActor_c.h: the key
       function pins where mwcc anchors the vtable. */
    /* Inline plus vtable instantiation is load-bearing: mwcc emits retail's
       D1 then D0 pair, with no homeless D2. InitResources is the first
       out-of-line virtual and anchors this TU's vtable/RTTI group. */
    virtual ~daEyBm_c() {}

    virtual s32  InitResources();       /* slot 0 */
    virtual s32  CleanupResources();    /* slot 3 */
    virtual s32  Behavior();            /* slot 6 */
    virtual s32  Render();              /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 */

private:
    /* Class ownership, calls, bodies and codegen are proven. These readable
       private spellings are inferred aliases; no original names survive. */
    void SpawnDestroyEffect();
    void UpdateCollision(dBgCh_Actr &collision);
    void HurtPlayer();
    void UpdateShadow();
};

typedef char daEyBm_c_size_must_be_0x334[
    sizeof(daEyBm_c) == 0x334 ? 1 : -1];

/* InitResources owns the compiler-emitted definition of this vtable. The
 * measured factory must store its public address point directly because
 * natural new selects the wrong allocator; this declaration only exposes
 * that compiler-owned address to the factory seam. */
extern int _ZTV8daEyBm_c[];

#endif /* DAEYBM_C_H */
