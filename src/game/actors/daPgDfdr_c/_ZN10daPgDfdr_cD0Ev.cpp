//cpp
// @symbol _ZN10daPgDfdr_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases,
 * then return the object to its heap. Nobody writes that; declaring
 * `~daPgDfdr_c()` is enough, because mwcc emits D2, D0 and D1 together and
 * objisolate keeps the one this file is bound to.
 *
 * The deallocation is an inline operator delete, found by ordinary lookup on
 * dActor_c (include/dActor_c.h) two levels up -- the same shape
 * include/BigBrickBlock.h's D0 and src/_ZN7daDgr_cD0Ev.cpp already proved for
 * leaves below dActor_c through dBgActor_c -- which is why nothing below
 * mentions a heap.
 */
#include "daPgDfdr_c.h"

daPgDfdr_c::~daPgDfdr_c()
{
}
