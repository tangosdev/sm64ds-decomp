//cpp
/* PROBE, not for landing. Can one real C++ class reproduce all five of
 * dM3dGSph's structor bodies at 0x0203ac1c..0x0203ac7f, so that the compiler
 * emits _ZTV8dM3dGSph itself instead of the tree hand-storing data_020994cc?
 *
 * ROM span, five contiguous bodies:
 *   0x0203ac1c  0x10  D2
 *   0x0203ac2c  0x24  D0   vptr, Memory::operator_delete2(this), return this
 *   0x0203ac50  0x10  D1
 *   0x0203ac60  0x10  C1
 *   0x0203ac70  0x10  C2
 */
#include "types.h"

extern "C" void _ZN6Memory16operator_delete2EPv(void *ptr);

struct dM3dGSph {
    Vector3 centre;         /* 0x04 */
    Fix12i radius;          /* 0x10 */

    dM3dGSph();
    virtual ~dM3dGSph();

    /* Inline, in the class itself -- mwcc inlines it only when it finds it here
       or one level up (include/Animation.h:86, include/dActor_c.h). */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

dM3dGSph::dM3dGSph() {}
dM3dGSph::~dM3dGSph() {}
