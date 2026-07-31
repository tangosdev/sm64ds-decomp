// @symbol func_ov060_021181e8
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daKpa3Bg_c.h"
// @emits daKpa3Bg_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daKpa3Bg_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void *data_ov060_02119514[];
extern void *data_ov060_0211953c[];
int daKpa3Bg_c_CleanupResources(char *c)
{
    struct daKpa3Bg_c *self = (struct daKpa3Bg_c *)(void *)c;
    _ZN16MeshColliderBase7DisableEv(c + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov060_02119514[self->unk_329]);
    _ZN13SharedFilePtr7ReleaseEv(data_ov060_0211953c[self->unk_329]);
    return 1;
}
