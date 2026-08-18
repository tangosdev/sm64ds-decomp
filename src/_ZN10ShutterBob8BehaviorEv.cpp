//cpp
// @symbol _ZN10ShutterBob8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShutterBob.h"
/* The local `class dBgActor_c { void UpdateClsnPosAndRot(); };` this file used to
   carry is gone: ShutterBob.h now reaches the real one through daObjSwdoor_c, and
   two definitions of the name in one translation unit do not compile. The call is
   the same call -- dBgActor_c::UpdateClsnPosAndRot is non-virtual, so it is a direct
   `bl` either way. */

int ShutterBob::Behavior()
{
int r4 = func_ov002_020bac18();
UpdateClsnPosAndRot();
return r4;
}
