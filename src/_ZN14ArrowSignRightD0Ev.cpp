//cpp
// @symbol _ZN14ArrowSignRightD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~ArrowSignRight()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "ArrowSignRight.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~ArrowSignRight() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define it twice. The port builds this
 * class from the header's flat host view (its SM64DS_PLATFORM_PC branch),
 * which has no base class and so no inherited operator delete. The ROM's
 * D0 ends in dActor_c's inline delete, Memory::Deallocate on the actor heap
 * word at 0x020a0eac, and this arm makes that call by name. So it makes
 * the two calls the port's generated face for this D0 made: the D1 body,
 * called qualified so it is a direct call to the one host symbol, then
 * that deallocation. Nothing here reaches mwccarm: it builds the `#else`
 * arm and emits the ROM bytes it always emitted, and the object is
 * byte-identical either way. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;
extern "C" ArrowSignRight *_ZN14ArrowSignRightD0Ev(ArrowSignRight *thiz)
{
    thiz->ArrowSignRight::~ArrowSignRight();                /* the D1 body, through the one host symbol */
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);   /* the actor-heap delete D0 ends with */
    return thiz;
}
#else
ArrowSignRight::~ArrowSignRight()
{
}
#endif
