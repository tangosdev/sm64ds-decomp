//cpp
// @symbol _ZN9BootSceneD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version this replaces spelled out three vptr stores --
 * dScBoot_c's own vtable, then _ZTV5Scene, then ActorDerived's -- followed by
 * a call to ActorBase::~ActorBase. That is the chain with the two
 * intermediate destructors inlined: BootScene's own, then Scene's (which
 * itself inlines ActorDerived's, see _ZN5SceneD1Ev.cpp), then a real call to
 * ActorBase's, because that one is not defined inline anywhere in the chain.
 *
 * BootScene adds no members with destructors, so there is nothing between
 * them. It was also misnamed and miscounted before this slice -- see the
 * comment at the top of include/BootScene.h and notes/dtor-variant-audit.md:
 * this symbol was found as `_ZN5SceneD2Ev` in dScBoot_c's own vtable slot 16,
 * which is impossible twice over (wrong class, and a D2 is never in a vtable
 * at all).
 */
#include "BootScene.h"

BootScene::~BootScene()
{
}
