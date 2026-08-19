// @symbol _ZN11daBgSnwmn_c16CleanupResourcesEv
/* daBgSnwmn_c::CleanupResources() -- vtable slot 3. See include/daBgSnwmn_c.h. */
#include "decl_common.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov072_02122c48[];
int _ZN11daBgSnwmn_c16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c48);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c40);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c50);
    return 1;
}
