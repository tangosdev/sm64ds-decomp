//cpp
// @symbol _ZN5SceneD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~Scene is defined in the class body rather than here, and that is not a
 * style choice -- Stage::~Stage and every one of Scene's other nine direct
 * children inline this destructor's vptr store, which the compiler can only
 * do from a visible body. include/Scene.h records the measurement.
 *
 * So this file cannot define it: that would be a redefinition. But the
 * vtable points at an out-of-line copy, and a TU that merely includes the
 * header emits nothing at all -- `_ZN5SceneD1Ev is not in the object`. The
 * explicit destructor call below is what forces the copy into existence.
 *
 * It is never called. mwcc emits the destructor group when it sees the call,
 * objisolate keeps the one this file declares and drops the rest, and the
 * bytes at 0x0202e140 come out exactly as the ROM has them.
 */
#include "Scene.h"

/* Not called. Forces the out-of-line copy of the inline destructor above. */
void Scene_EmitDestructor(Scene *p)
{
    p->~Scene();
}
