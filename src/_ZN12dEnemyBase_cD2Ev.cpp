//cpp
// @symbol _ZN12dEnemyBase_cD2Ev
/* recovered: real C++ base-object destructor -- the compiler emits the whole body
 *
 * D2 is the BASE-OBJECT destructor: store this class's vtable over the one
 * dActor_c's constructor left, then run the dActor_c subobject destructor. It is
 * the variant the 51 derived classes call from their own destructors, which is
 * why the ROM keeps it distinct from D1.
 *
 * Nobody writes any of that. One `dEnemyBase_c::~dEnemyBase_c() {}` makes mwcc emit
 * D2, D0 and D1 together; objisolate keeps the one this file is bound to. The
 * sibling files _ZN12dEnemyBase_cD0Ev.cpp and _ZN12dEnemyBase_cD1Ev.cpp carry the
 * identical body for the same reason -- that is not duplication, it is how the
 * one-symbol-per-file enrolment meets a compiler that emits three.
 */
#include "dEnemyBase_c.h"

dEnemyBase_c::~dEnemyBase_c()
{
}
