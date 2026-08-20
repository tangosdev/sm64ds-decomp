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
 *   slot 3  0x02014a20  INHERITED, see below
 *
 * Derives from dCcAc_c, not from dCc_c directly: its
 * typeinfo at 0x0208e680 is the 12-byte __si_class_type_info kind whose base
 * pointer resolves to dCcAc_c, and the ROM names this class dCcAcPos_c. C1
 * agrees -- it calls dCcAc_c's C2, then stores this vtable.
 *
 * SLOT 3 IS NOT AN OVERRIDE. func_02014a20 is a 12-byte interworking veneer,
 * `ldr ip,[pc]; bx ip; .word 0x0201493c', and that literal is
 * _ZN7dCcAc_c10GetOwnerIDEv -- the parent's body. The linker put
 * the veneer there; the class inherits GetOwnerID unchanged, so it is
 * deliberately NOT redeclared below. Declaring it would invent an override
 * the ROM does not have.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 stays a C file.
 *
 * LAYOUT: dCcAc_c is 0x34 and pos starts at exactly 0x34, so the
 * object is 0x40.
 *
 * GetPos here shadows the parent's: dCcAc_c returns the owner's
 * position, this one returns its own field.
 */

#ifdef __cplusplus

struct dCcAcPos_c : dCcAc_c {
    Vector3 pos;            /* 0x34 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dCcAcPos_c();   /* slots 0 (D1), 1 (D0) */
    virtual Vector3 &GetPos();              /* slot 2 - our pos, not the owner's */
    /* slot 3 GetOwnerID is inherited through a linker veneer; see above. */

    /* --- non-virtual --- */
    void SetPosRelativeToActor(const Vector3 &offset);
};

typedef char dCcAcPos_c_size_must_be_0x40[
    sizeof(dCcAcPos_c) == 0x40 ? 1 : -1];

#else

struct dCcAcPos_c {
    struct dCcAc_c base; /* 0x00 */
    Vector3 pos;                    /* 0x34 */
};

#endif /* __cplusplus */

#endif /* DCCACPOS_C_H */
