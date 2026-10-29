#ifndef DCCACPOS_C_H
#define DCCACPOS_C_H

#include "types.h"
#include "dCcAc_c.h"

/* An dActor_c-attached cylinder that keeps its own position, vtable
 * _ZTV10dCcAcPos_c at 0x0208e704.
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x02014a60  ~dCcAcPos_c (D1)
 *   slot 1  0x02014a34  ~dCcAcPos_c (D0)
 *   slot 2  0x02014a2c  GetPos()     - `add r0, r0, #0x34', this->pos
 *   slot 3  0x02014a20  GetOwnerID() - the veneer, see below
 *
 * Derives from dCcAc_c, not from dCc_c directly: its
 * typeinfo at 0x0208e680 is the 12-byte __si_class_type_info kind whose base
 * pointer resolves to dCcAc_c, and the ROM names this class dCcAcPos_c. C1
 * agrees -- it calls dCcAc_c's C2, then stores this vtable.
 *
 * SLOT 3 STAYS THE VENEER. The cartridge word is 0x02014a20, twelve bytes:
 * `ldr ip,[pc]; bx ip; .word _ZN7dCcAc_c10GetOwnerIDEv`. It is not the base
 * body at 0x0201493c. The definition is the qualified call; -interworking
 * lowers that call to the veneer, and declaring the override here is what
 * makes the key function's vtable bind the slot to the veneer. Leaving the
 * function inherited binds slot 3 to dCcAc_c::GetOwnerID and the emitted
 * table no longer matches the cartridge.
 *
 * THE DESTRUCTOR IS DECLARED FIRST. One ~dCcAcPos_c() in
 * src/engine/collision/d_cc_ac.cpp emits D2, D0 and D1. D2 has no ROM home.
 * See include/dExtModel_c.h for the key-function rule.
 *
 * LAYOUT: dCcAc_c is 0x34 and pos starts at exactly 0x34, so the
 * object is 0x40.
 *
 * GetPos here shadows the parent's: dCcAc_c returns the owner's
 * position, this one returns its own field.
 */

#ifdef __cplusplus

#include "math/Fix12.h"

struct dCcAcPos_c : dCcAc_c {
    Vector3 pos;            /* 0x34 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dCcAcPos_c();   /* slots 0 (D1), 1 (D0) */
    virtual Vector3 &GetPos();              /* slot 2 - our pos, not the owner's */
    virtual u32 GetOwnerID();               /* slot 3 - veneer to dCcAc_c::GetOwnerID */

    /* Defined out of line in src/engine/collision/d_cc_ac.cpp. The body is
     * empty; C1 is the dCcAc_c base step and the vptr store. C2 is the
     * homeless base-object variant of that same definition. */
    dCcAcPos_c();

    /* --- non-virtual --- */
    void Init(dActor_c *actor, const Vector3 &offset, Fix12i radius, Fix12i height, u32 flags,
              u32 vulnFlags);
    void SetPosRelativeToActor(const Vector3 &offset);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dCcAcPos_c_size_must_be_0x40[
    sizeof(dCcAcPos_c) == 0x40 ? 1 : -1];
#endif

#else

struct dCcAcPos_c {
    struct dCcAc_c base; /* 0x00 */
    Vector3 pos;                    /* 0x34 */
};

/* In C the tag alone is not a type name, so an owner header that embeds a
   dCcAcPos_c BY VALUE -- which the cartridge's own destructors prove several do,
   see tools/dtor_members.py -- cannot spell the member without this. The
   definition and the typedef have to travel together: with the definition and no
   typedef the embed gets `undefined identifier', and then the owner's size assert
   gets `illegal constant expression' on top of it. */
typedef struct dCcAcPos_c dCcAcPos_c;

/* The C view substitutes for the C++ class only while it is the SAME SIZE. Once
   an owner embeds one by value the two branches lay that owner out differently if
   they ever disagree, and nothing else in the build compares them. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dCcAcPos_c_size_must_be_0x40[
    sizeof(struct dCcAcPos_c) == 0x40 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DCCACPOS_C_H */
