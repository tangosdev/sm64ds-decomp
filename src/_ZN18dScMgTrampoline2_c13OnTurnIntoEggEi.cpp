//cpp
// @symbol _ZN18dScMgTrampoline2_c13OnTurnIntoEggEi
// recovered name: dScMgTrampoline2_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgTrampoline2_c.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
extern int GetGameLanguage(void);
}
int dScMgTrampoline2_c::OnTurnIntoEgg(int /* mode */)
{
    func_ov006_020c8a9c(0, data_ov006_0213fc20[GetGameLanguage()]);
    return 1;
}
