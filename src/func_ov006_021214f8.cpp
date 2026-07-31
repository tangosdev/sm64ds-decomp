//cpp
// @symbol func_ov006_021214f8
// @emits dScMgTrampoline_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline_c::Behavior - recovered from vtable slot identity */
struct C;
typedef void (C::*PMF)();
struct C {
    char pad[0x5004];
    PMF pmf;
};
extern "C" {
}

extern "C" int dScMgTrampoline_c_Behavior(char *c)
{
    int saved = data_ov006_02140588;
    func_ov006_02120c40();
    (((C *)c)->*(((C *)c)->pmf))();
    func_ov006_0212157c(c);
    func_ov006_021209ac((short *)(c + 0x5d84));
    if (saved != data_ov006_02140588)
        func_ov004_020adb1c(data_ov006_02140588);
    return 1;
}
