//cpp
// @symbol _ZN10HootTheOwlD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~HootTheOwl()`
 * is enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to.
 *
 * The deallocation is an inline operator delete -- dEnemyBase_c's, reached because
 * dEnemyBase_c is this class's IMMEDIATE base -- which is why nothing below mentions a
 * heap.
 *
 * The C original stored `_ZTV7daOwl_c` here where D1 stored `_ZTV10HootTheOwl`.
 * Those are two names for one address (ov094 0x02136a58, both in symbols.txt),
 * EAD's internal name and the tree's English one, so the difference was only
 * ever in the source.
 */
#include "HootTheOwl.h"

HootTheOwl::~HootTheOwl()
{
}
