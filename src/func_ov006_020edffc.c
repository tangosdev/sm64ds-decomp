// @symbol func_ov006_020edffc
// recovered name: dScMgJump_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgJump_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *p);
extern void func_ov004_020ad90c(void);

int func_ov006_020edffc(void) {
    _ZN13SharedFilePtr7ReleaseEv(data_ov006_02142184);
    data_ov006_02142184 = 0;
    func_ov004_020ad90c();
    return 1;
}
