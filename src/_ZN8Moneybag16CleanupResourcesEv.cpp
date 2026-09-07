//cpp
// @symbol _ZN8Moneybag16CleanupResourcesEv
/* Moneybag::CleanupResources -- vtable slot 3. Releases one shared file, then a
 * four-entry table; it never touches `this`. */
#include "Moneybag.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern void *data_ov081_02128ed4;
extern void *data_ov081_021280d8[];
}

int Moneybag::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov081_02128ed4);
    for (int i = 0; i < 4; i++) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov081_021280d8[i]);
    }
    return 1;
}
