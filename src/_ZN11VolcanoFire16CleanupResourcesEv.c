#include "types.h"
struct VolcanoFire {
    char pad[0x10c];
    char *sub;
};

int _ZN11VolcanoFire16CleanupResourcesEv(struct VolcanoFire *self)
{
    char *p = self->sub;
    if (p)
        *(u16 *)(p + 0x324) -= 1;
    return 1;
}
