// @symbol _ZN9UkikiCage16CleanupResourcesEv
/* recovered: renamed to Class_Method, declarations from a shared header
 *
 * UkikiCage::CleanupResources -- vtable slot 3, ov030 0x0211130c. Attributed
 * by the vtable: _ZTV9UkikiCage carries this address at slot 3, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043bf0.
 *
 * Kept as a free function under the literal mangled name rather than a real
 * method, same shape as _ZN9UkikiCage13InitResourcesEv.cpp. */
#include "decl_common.h"
extern int data_ov030_02115c80[];
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov030_02115c88[];
int _ZN9UkikiCage16CleanupResourcesEv(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov030_02115c88);
    _ZN13SharedFilePtr7ReleaseEv(data_ov030_02115c80);
    return 1;
}
