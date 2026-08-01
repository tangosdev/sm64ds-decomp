#include "types.h"
struct RollingLogLll {
    char pad[0x10c];
    char *sub;
};

int _ZN13RollingLogLll16CleanupResourcesEv(struct RollingLogLll *self)
{
    char *p = self->sub;
    if (p)
        *(u16 *)(((long long)(int)(p + 0x324))) -= 1;
    return 1;
}
