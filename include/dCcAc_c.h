#ifndef DCCAC_C_H
#define DCCAC_C_H

#include "types.h"
#include "dCc_c.h"

/* A cylinder attached to an dActor_c, vtable _ZTV7dCcAc_c at
 * 0x0208e6d4.
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x020149a4  ~dCcAc_c (D1)
 *   slot 1  0x02014978  ~dCcAc_c (D0)
 *   slot 2  0x02014948  GetPos()      - `ldr r0,[r0,#0x30]' then + 0x5c
 *   slot 3  0x0201493c  GetOwnerID()  - `ldr r0,[r0,#0x30]; ldr r0,[r0,#4]'
 *
 * Derives from dCc_c: its typeinfo at 0x0208e698 is the 12-byte
 * __si_class_type_info kind whose base pointer resolves to dCc_c, and the
 * ROM names this class dCcAc_c. C2 confirms it -- it calls dCc_c's C2,
 * stores this vtable, then nulls the owner.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 and D2 stay C files.
 *
 * LAYOUT: the base is 0x30 and owner sits at 0x30, so the object is 0x34 --
 * which is where dCcAcPos_c starts its own field.
 *
 * BOTH VIRTUALS READ THROUGH owner. GetOwnerID loads owner->uniqueID at
 * dActor_c + 4, the same offset dBgW uses. GetPos does NOT return a
 * field of this object at all: it returns the owner's position at dActor_c +
 * 0x5c, so a moving cylinder tracks its dActor_c rather than storing a copy.
 */

#ifdef __cplusplus

#include "math/Fix12.h"

struct dActor_c;

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct dCcAc_c : dCc_c {
    dActor_c *owner;           /* 0x30 - nulled by C2 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dCcAc_c();      /* slots 0 (D1), 1 (D0) */
    virtual Vector3 &GetPos();          /* slot 2 - the owner's pos, not ours */
    virtual u32 GetOwnerID();           /* slot 3 - owner->uniqueID */

    /* DECLARED, defined out of line in src/_ZN7dCcAc_cC1Ev.cpp and
     * src/_ZN7dCcAc_cC2Ev.cpp as real C++ -- the init list nulls owner, which
     * is the only store past the vptr the ROM makes. */
    dCcAc_c();

    /* --- non-virtual --- */
    void Init(dActor_c *actor, Fix12<int> radius, Fix12<int> height, u32 flags, u32 vulnFlags);

    /* WHAT LETS A REAL `~Class()` REPRODUCE THE ROM'S DELETING DESTRUCTOR.
       The compiler generates D0 as "run the destructor body, then call operator
       delete on the class". Without this it emits the global `_ZdlPv`, which
       exists nowhere in this image, and the D0 comes out one relocated word
       different from the ROM -- a difference build_pin.verify CANNOT SEE,
       because it wildcards relocated words. Only the link catches it.

       This family deallocates through Memory::operator_delete2, not the actor
       heap: every D0 below ends with a call to 0x0203cbcc. dActor_c's copy of this
       member calls Memory::Deallocate instead, which is why each needs its own.

       Inline, and in the IMMEDIATE base -- mwcc inlines it only when it finds it
       in the class or one level up, as include/dActor_c.h records. No layout
       effect: a non-virtual inline member adds no field and no vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

typedef char dCcAc_c_size_must_be_0x34[
    sizeof(dCcAc_c) == 0x34 ? 1 : -1];

#else

struct dCcAc_c {
    struct dCc_c base; /* 0x00 */
    struct dActor_c *owner;      /* 0x30 */
};

/* In C the tag alone is not a type name, so an owner header that embeds a
   dCcAc_c BY VALUE -- which the cartridge's own destructors prove several do,
   see tools/dtor_members.py -- cannot spell the member without this. The
   definition and the typedef have to travel together: with the definition and no
   typedef the embed gets `undefined identifier', and then the owner's size assert
   gets `illegal constant expression' on top of it. */
typedef struct dCcAc_c dCcAc_c;

/* The C view substitutes for the C++ class only while it is the SAME SIZE. Once
   an owner embeds one by value the two branches lay that owner out differently if
   they ever disagree, and nothing else in the build compares them. */
typedef char dCcAc_c_size_must_be_0x34[sizeof(struct dCcAc_c) == 0x34 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DCCAC_C_H */
