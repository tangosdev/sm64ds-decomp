#ifndef DAOBJC1HIKARI_C_H
#define DAOBJC1HIKARI_C_H

#include "dActor_c.h"
#include "Model.h"
#include "dCcAcPos_c.h"

/**
 * Castle light beam (profile C1_HIKARI / LIGHT_BEAM(37)).
 *
 * ROM RTTI ov010:0x02112b64 names daObjC1Hikari_c as a direct dActor_c
 * subclass. daObjC1Hikari_c_classInit allocates 0x16c.
 *
 * UpdateModelTransform is an inferred private helper: class ownership,
 * the InitResources call, and codegen are proven; the original English
 * spelling is not.
 */
struct daObjC1Hikari_c : dActor_c {
    u8 pad_0d0[4];                    /* 0x0d0 */
    Model mModel;                     /* 0x0d4 */
    dCcAcPos_c mCylinder;             /* 0x124 */
    dActor_c *mTalkPlayer;            /* 0x164 */
    u16 mSoundTimers[2];              /* 0x168 */

    /* Inline so InitResources can own the vtable and emit retail's D1 then D0
       pair without an extra D2. */
    virtual ~daObjC1Hikari_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    void UpdateModelTransform();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjC1Hikari_c_size_must_be_0x16c[
    sizeof(daObjC1Hikari_c) == 0x16c ? 1 : -1];
#endif

#endif
