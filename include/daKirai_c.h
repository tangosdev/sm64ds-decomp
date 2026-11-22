/* daKirai_c: the spike bomb. 5 matched functions, sizeof 0x1b0.
 *
 * Layout, each sub-object checked by its own size assertion and by closing
 * exactly on the next named field:
 *
 *     dActor_c     0x000 + 0x0d4 = 0x0d4   -> mModel
 *     dExtSimpleModel_c        0x0d4 + 0x050 = 0x124   -> mdCcAcPos_c
 *     dCcAcPos_c   0x124 + 0x040 = 0x164   -> padding, then mStateIndex
 *
 * The class's own destructor calls dCcAcPos_c's D1 at +0x124, which is what
 * makes mdCcAcPos_c an object rather than a marker byte with padding behind it.
 *
 * sizeof is the field span: the last field mOpacity ends at 0x1af, and
 * daKirai_c_classInit (the factory that stores _ZTV9daKirai_c) asks
 * fBase_c::operator new for 0x1b0. An earlier 0x32c came from
 * daKpa3Bg_c_classInit, a different class's factory. */
#ifndef DAKIRAI_C_H
#define DAKIRAI_C_H
#include "types.h"
#include "dExtSimpleModel_c.h"
#include "dCcAcPos_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct daKirai_c : dActor_c {
    /* Named by the class's own destructor calling dExtSimpleModel_c's D1 at +0x0d4 -- a
       relocation the ROM build checks. */
    dExtSimpleModel_c mModel; /* 0x0d4 */
    dCcAcPos_c mdCcAcPos_c;   /* 0x124 */
    u8 pad_164[0xc];
    s32 mStateIndex; /* 0x170 */
    Vector3 mHomePos; /* 0x174 */
    /* Both taken in InitResources. mHomeHorzDist is Vec3_HorzLen of the spawn
       position -- how far out in XZ the bomb starts from the world origin.
       mHomeYOffset is the constant 0x2ee000, and mHomePos.y is raised by
       mHomeYOffset >> 3 right after the home triple is copied.
       [daKirai_c::InitResources] */
    s32 mHomeHorzDist; /* 0x180 */
    s32 mHomeYOffset;  /* 0x184 */
    /* uniqueIDs of the other spike bombs (actor 0x11c), taken when this one
       disarms; zeroed as each one is found gone. [Disarm] */
    u32 mOtherBombIDs[8]; /* 0x188 */
    s32 mSlotIndex;       /* 0x1a8 -- AddSpikeBomb's slot */
    u16 mTimer;           /* 0x1ac -- frames into the swell/explode states */
    u8 mOpacity;          /* 0x1ae */
    u8 pad_1af[0x1];      /* 0x1af, closing on the ROM's 0x1b0 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~daKirai_c(); /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int CleanupResources();
    int Behavior();
    int InitResources();
    int Render();

    /* The bomb's helpers. The state table Behavior dispatches stores the four
       State* members as member pointers. */
    void Disarm();
    int IsInTriggerRange(Vector3 *pos);
    void Explode();
    void UpdateModelTransform();
    void Rearm();
    void StateSpent();
    void StateExploding();
    void StateSwelling();
    void StateArmed();
};

typedef char daKirai_c_size_must_be_0x1b0[sizeof(daKirai_c) == 0x1b0 ? 1 : -1];

#endif /* __cplusplus */

#endif
