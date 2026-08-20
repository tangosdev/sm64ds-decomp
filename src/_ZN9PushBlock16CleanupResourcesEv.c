// @symbol _ZN9PushBlock16CleanupResourcesEv
/* recovered: renamed to Class_Method, declarations from a shared header
 *
 * PushBlock::CleanupResources -- vtable slot 3, ov002 0x020b8d68. Attributed
 * by the vtable: _ZTV9PushBlock carries this address at slot 3, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043bf0.
 *
 * Kept as an extern free function under the literal mangled name rather than
 * a real method -- same shape as _ZN9PushBlock13InitResourcesEv.cpp. */
#include "decl_common.h"
extern int data_ov002_0210df94[];
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210df9c[];
int _ZN9PushBlock16CleanupResourcesEv(void *t)
{
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210df9c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210df94);
    return 1;
}
