//cpp
// @symbol _ZN10StarSwitch15OnGroundPoundedER8dActor_c
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "StarSwitch.h"
extern "C" {
extern void func_ov002_020ba4d8(char *c, int i);
}

void StarSwitch::OnGroundPounded(dActor_c &other)
{
    if (unk_340 != 0) return;
    func_ov002_020ba4d8(((char *)this), 1);
}
