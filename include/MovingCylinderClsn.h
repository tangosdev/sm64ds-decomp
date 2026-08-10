#ifndef MOVINGCYLINDERCLSN_H
#define MOVINGCYLINDERCLSN_H

#include "types.h"
#include "CylinderClsn.h"

/* A cylinder attached to an Actor, vtable _ZTV18MovingCylinderClsn at
 * 0x0208e6d4.
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x020149a4  ~MovingCylinderClsn (D1)
 *   slot 1  0x02014978  ~MovingCylinderClsn (D0)
 *   slot 2  0x02014948  GetPos()      - `ldr r0,[r0,#0x30]' then + 0x5c
 *   slot 3  0x0201493c  GetOwnerID()  - `ldr r0,[r0,#0x30]; ldr r0,[r0,#4]'
 *
 * Derives from CylinderClsn: its typeinfo at 0x0208e698 is the 12-byte
 * __si_class_type_info kind whose base pointer resolves to dCc_c, and the
 * ROM names this class dCcAc_c. C2 confirms it -- it calls CylinderClsn's C2,
 * stores this vtable, then nulls the owner.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 and D2 stay C files.
 *
 * LAYOUT: the base is 0x30 and owner sits at 0x30, so the object is 0x34 --
 * which is where MovingCylinderClsnWithPos starts its own field.
 *
 * BOTH VIRTUALS READ THROUGH owner. GetOwnerID loads owner->uniqueID at
 * Actor + 4, the same offset MeshColliderBase uses. GetPos does NOT return a
 * field of this object at all: it returns the owner's position at Actor +
 * 0x5c, so a moving cylinder tracks its Actor rather than storing a copy.
 */

#ifdef __cplusplus

struct Actor;

struct MovingCylinderClsn : CylinderClsn {
    Actor *owner;           /* 0x30 - nulled by C2 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~MovingCylinderClsn();      /* slots 0 (D1), 1 (D0) */
    virtual Vector3 &GetPos();          /* slot 2 - the owner's pos, not ours */
    virtual u32 GetOwnerID();           /* slot 3 - owner->uniqueID */
};

typedef char MovingCylinderClsn_size_must_be_0x34[
    sizeof(MovingCylinderClsn) == 0x34 ? 1 : -1];

#else

struct MovingCylinderClsn {
    struct CylinderClsn base; /* 0x00 */
    struct Actor *owner;      /* 0x30 */
};

#endif /* __cplusplus */

#endif /* MOVINGCYLINDERCLSN_H */
