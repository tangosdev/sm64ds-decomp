//cpp
// @symbol _ZN17dScMgTrampoline_c13OnTurnIntoEggEi
// recovered name: dScMgTrampoline_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgTrampoline_c.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
extern int GetGameLanguage(void);
}
int dScMgTrampoline_c::OnTurnIntoEgg(int /* mode */)
{
    func_ov006_020c8a9c(0, data_ov006_0213fb18[GetGameLanguage()]);
    return 1;
}
