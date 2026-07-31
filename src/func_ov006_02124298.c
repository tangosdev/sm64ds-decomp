// @symbol func_ov006_02124298
// @emits dScMgTrampoline2_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern int GetGameLanguage(void);
int dScMgTrampoline2_c_OnTurnIntoEgg(void)
{
    func_ov006_020c8a9c(0, data_ov006_0213fc20[GetGameLanguage()]);
    return 1;
}
