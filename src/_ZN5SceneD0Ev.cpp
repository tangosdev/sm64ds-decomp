//cpp
// @symbol _ZN5SceneD0Ev
/* recovered: real C++ deleting destructor, forced from the inline definition
 *
 * ~Scene is defined in the class body (include/Scene.h), not here -- see the
 * long note there and in _ZN5SceneD1Ev.cpp: Stage and Scene's nine other
 * direct children inline Scene's own D2, which the compiler can only do from
 * a visible body. This file cannot define ~Scene() again (redefinition), so
 * a delete-expression forces the deleting destructor's own out-of-line copy
 * into existence instead -- the same role _ZN5SceneD1Ev.cpp's plain
 * destructor call plays for D1/D2.
 *
 * It is never called. objisolate keeps the symbol this file declares and
 * drops the rest, and the bytes at 0x0202e170 come out exactly as the ROM
 * has them: Scene's own vptr, then ActorDerived's (inlined), then
 * ActorBase::~ActorBase(), then Scene's own operator delete.
 */
#include "Scene.h"

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void Scene_EmitDeletingDestructor(Scene *p)
{
    delete p;
}
