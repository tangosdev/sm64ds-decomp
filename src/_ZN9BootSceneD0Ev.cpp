//cpp
// @symbol _ZN9BootSceneD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through BootScene (which inlines ~dScene_c and ~dBase_c, see
 * _ZN9BootSceneD1Ev.cpp), then a real call to fBase_c::~fBase_c, then
 * hand the object back through dScene_c's inline operator delete -- declared on
 * dScene_c, BootScene's IMMEDIATE base, which is where mwcc finds it (it only
 * inlines an operator found on the class itself or its immediate base). The
 * hand-written version declared its own local `data_020a0eac` for the actor
 * heap, which collides with the `void *` dScene_c.h now supplies for that same
 * symbol.
 */
#include "BootScene.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~BootScene() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" BootScene *_ZN9BootSceneD0Ev(BootScene *thiz)
{
    thiz->BootScene::~BootScene();     /* the D1 body, through the one host symbol */
    BootScene::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
BootScene::~BootScene()
{
}
#endif
