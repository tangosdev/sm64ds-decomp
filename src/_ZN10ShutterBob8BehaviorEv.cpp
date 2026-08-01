//cpp
// @symbol _ZN10ShutterBob8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShutterBob.h"
class Platform {
public:
void UpdateClsnPosAndRot();
};


int ShutterBob::Behavior()
{
int r4 = func_ov002_020bac18();
((Platform *)this)->UpdateClsnPosAndRot();
return r4;
}
