//cpp
// @symbol _ZN17BigMovingIceBlockD0Ev
/* Real compiler-spelled deleting destructor. The delete-expression selects
 * D0 and reaches dActor_c's inline actor-heap deallocator after the complete
 * BigMovingIceBlock destruction sequence. */
#include "BigMovingIceBlock.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~BigMovingIceBlock()
 * is defined in the class body, so a qualified call from here would inline
 * a second copy of the whole destructor chain into this arm. The port's D1
 * for this class is its flat ROM name, the name the class's vtable slot 16
 * holds, so this arm makes the D0's two calls: the D1 body through that
 * flat name, then the class-specific operator delete. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN17BigMovingIceBlockD1Ev(void *self);
extern "C" BigMovingIceBlock *_ZN17BigMovingIceBlockD0Ev(BigMovingIceBlock *thiz)
{
    _ZN17BigMovingIceBlockD1Ev(thiz);          /* the D1 body, through its flat ROM name */
    BigMovingIceBlock::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void BigMovingIceBlock_EmitDeletingDestructor(BigMovingIceBlock *p)
{
    delete p;
}
#endif
