//cpp
// @symbol _ZN5StageD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through Stage and Scene (see _ZN5StageD1Ev.cpp), then hand the
 * object back through Scene's inline operator delete -- declared there
 * rather than on ActorDerived, because mwcc only inlines it when found on
 * the class itself or its IMMEDIATE base, which for Stage is Scene.
 */
#include "Stage.h"

Stage::~Stage()
{
}
