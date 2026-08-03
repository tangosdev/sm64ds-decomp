//cpp
// @symbol _ZN5Pokey8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Pokey.h"
extern "C" {
extern int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void *c, int d);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void *c, void *cyl);
}

int Pokey::Behavior()
{
    int s = unk_38c;
    if (s != 2 && s != 5) {
        if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), 0x5dc000)) return 1;
    }
    func_ov096_021368b4(((char *)this));
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char *)this), ((char *)this) + 0x14c);
    func_ov096_02135efc(((char *)this));
    return 1;
}
