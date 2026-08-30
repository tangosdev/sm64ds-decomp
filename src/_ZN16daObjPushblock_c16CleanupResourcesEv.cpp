//cpp
// @symbol _ZN16daObjPushblock_c16CleanupResourcesEv
/* recovered: real C++ method */
/* daObjPushblock_c::CleanupResources() -- takes the block's dBgW collision out of the
 * world if it is still registered, then gives back the two shared files. */
#include "daObjPushblock_c.h"
#include "decl_common.h"

extern "C" {
extern int data_ov002_0210df94[];
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210df9c[];
}

int daObjPushblock_c::CleanupResources()
{
    if (_ZN4dBgW9IsEnabledEv((char *)this + 0x124)) {
        _ZN4dBgW7DisableEv((char *)this + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210df9c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210df94);
    return 1;
}
