#ifndef CYLINDERCLSNWITHPOS_H
#define CYLINDERCLSNWITHPOS_H

#include "types.h"
#include "CylinderClsn.h"

/* A cylinder that carries its own position, vtable _ZTV19CylinderClsnWithPos
 * at 0x0208e6bc.
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x02014854  ~CylinderClsnWithPos (D1)
 *   slot 1  0x02014828  ~CylinderClsnWithPos (D0)
 *   slot 2  0x02014820  GetPos()      - returns this->pos
 *   slot 3  0x02014818  GetOwnerID()  - `mov r0,#0; bx lr'
 *
 * Derives from CylinderClsn: its typeinfo at 0x0208e68c is the 12-byte
 * __si_class_type_info kind whose base pointer resolves to dCc_c, and the
 * ROM's own name for this class is dCcPos_c. C1 confirms it from the other
 * side -- it calls CylinderClsn's C2 and then stores this vtable.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelBase.h.
 *
 * LAYOUT: the base is 0x30, and pos starts at exactly 0x30 (GetPos is
 * `add r0, r0, #0x30'), so the object is 0x3c.
 *
 * Having no owning Actor is what slot 3 encodes: GetOwnerID is a constant 0,
 * where MovingCylinderClsn's reads through its owner pointer.
 */

#ifdef __cplusplus

struct CylinderClsnWithPos : CylinderClsn {
    Vector3 pos;            /* 0x30 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* The destructor pair as two plain virtuals -- the base's spelling, and it
       has to be the SAME two names here or MSVC appends a slot instead of
       overriding. include/CylinderClsn.h carries the measurement. */
    virtual void Destructor1();         /* slot 0 (D1) */
    virtual void Destructor0();         /* slot 1 (D0) */
    virtual Vector3 &GetPos();          /* slot 2 */
    virtual u32 GetOwnerID();           /* slot 3 - always 0 */

    /* --- non-virtual --- */
    /* Copies pos in and forwards to CylinderClsn::Init. Declared here so a
       caller can reach it without repeating the defining TU's local class
       shape; the definition is src/_ZN19CylinderClsnWithPos4InitERK7Vector3
       5Fix12IiES4_jj.c, which publishes the mangled name as a free function
       the way the destructors do. */
    void Init(const Vector3 &pos, Fix12i radius, Fix12i height, u32 flags,
              u32 vulnFlags);
};

typedef char CylinderClsnWithPos_size_must_be_0x3c[
    sizeof(CylinderClsnWithPos) == 0x3c ? 1 : -1];

#else

struct CylinderClsnWithPos {
    struct CylinderClsn base; /* 0x00 */
    Vector3 pos;              /* 0x30 */
};

#endif /* __cplusplus */

#endif /* CYLINDERCLSNWITHPOS_H */
