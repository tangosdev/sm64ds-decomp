//cpp
// @symbol _ZN9BootSceneD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version this replaces spelled out three vptr stores --
 * dScBoot_c's own vtable, then _ZTV8dScene_c, then dBase_c's -- followed by
 * a call to fBase_c::~fBase_c. That is the chain with the two
 * intermediate destructors inlined: BootScene's own, then dScene_c's (which
 * itself inlines dBase_c's, see _ZN8dScene_cD1Ev.cpp), then a real call to
 * fBase_c's, because that one is not defined inline anywhere in the chain.
 *
 * BootScene adds no members with destructors, so there is nothing between
 * them. It was also misnamed and miscounted before this slice -- see the
 * comment at the top of include/BootScene.h and notes/dtor-variant-audit.md:
 * this symbol was found as `_ZN8dScene_cD2Ev` in dScBoot_c's own vtable slot 16,
 * which is impossible twice over (wrong class, and a D2 is never in a vtable
 * at all).
 */
#include "BootScene.h"

BootScene::~BootScene()
{
}
