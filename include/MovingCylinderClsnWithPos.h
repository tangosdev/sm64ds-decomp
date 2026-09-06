#ifndef MOVINGCYLINDERCLSNWITHPOS_H
#define MOVINGCYLINDERCLSNWITHPOS_H

#include "types.h"
#include "MovingCylinderClsn.h"

/* An Actor-attached cylinder that keeps its own position, vtable
 * _ZTV25MovingCylinderClsnWithPos at 0x0208e704.
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x02014a60  ~MovingCylinderClsnWithPos (D1)
 *   slot 1  0x02014a34  ~MovingCylinderClsnWithPos (D0)
 *   slot 2  0x02014a2c  GetPos()     - `add r0, r0, #0x34', this->pos
 *   slot 3  0x02014a20  INHERITED, see below
 *
 * Derives from MovingCylinderClsn, not from CylinderClsn directly: its
 * typeinfo at 0x0208e680 is the 12-byte __si_class_type_info kind whose base
 * pointer resolves to dCcAc_c, and the ROM names this class dCcAcPos_c. C1
 * agrees -- it calls MovingCylinderClsn's C2, then stores this vtable.
 *
 * SLOT 3 IS NOT AN OVERRIDE. func_02014a20 is a 12-byte interworking veneer,
 * `ldr ip,[pc]; bx ip; .word 0x0201493c', and that literal is
 * _ZN18MovingCylinderClsn10GetOwnerIDEv -- the parent's body. The linker put
 * the veneer there; the class inherits GetOwnerID unchanged, so it is
 * deliberately NOT redeclared below. Declaring it would invent an override
 * the ROM does not have.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelBase.h.
 *
 * LAYOUT: MovingCylinderClsn is 0x34 and pos starts at exactly 0x34, so the
 * object is 0x40.
 *
 * GetPos here shadows the parent's: MovingCylinderClsn returns the owner's
 * position, this one returns its own field.
 */

#ifdef __cplusplus

struct MovingCylinderClsnWithPos : MovingCylinderClsn {
    Vector3 pos;            /* 0x34 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* The destructor pair as two plain virtuals -- the base's spelling, and it
       has to be the SAME two names here or MSVC appends a slot instead of
       overriding. include/CylinderClsn.h carries the measurement. */
    virtual void Destructor1();         /* slot 0 (D1) */
    virtual void Destructor0();         /* slot 1 (D0) */
    virtual Vector3 &GetPos();              /* slot 2 - our pos, not the owner's */
    /* slot 3 GetOwnerID is inherited through a linker veneer; see above. */

    /* --- non-virtual --- */
    void SetPosRelativeToActor(const Vector3 &offset);
};

typedef char MovingCylinderClsnWithPos_size_must_be_0x40[
    sizeof(MovingCylinderClsnWithPos) == 0x40 ? 1 : -1];

#else

struct MovingCylinderClsnWithPos {
    struct MovingCylinderClsn base; /* 0x00 */
    Vector3 pos;                    /* 0x34 */
};

#endif /* __cplusplus */

#endif /* MOVINGCYLINDERCLSNWITHPOS_H */
