//cpp
// @symbol _ZN13PoleBillboard16CleanupResourcesEv
#include "PoleBillboard.h"
// recovered name: daObjBkBillboard_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daObjBkBillboard_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov015_02114960[];
}

s32 PoleBillboard::CleanupResources() {
    _ZN13SharedFilePtr7ReleaseEv(data_ov015_02114960);
    return 1;
}
