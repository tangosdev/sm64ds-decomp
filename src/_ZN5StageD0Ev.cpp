//cpp
// @symbol _ZN5StageD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through Stage and dScene_c (see _ZN5StageD1Ev.cpp), then hand the
 * object back through dScene_c's inline operator delete -- declared there
 * rather than on dBase_c, because mwcc only inlines it when found on
 * the class itself or its IMMEDIATE base, which for Stage is dScene_c.
 */
#include "Stage.h"

Stage::~Stage()
{
}
