//cpp
// @symbol _ZN9MontyMole16CleanupResourcesEv
/* MontyMole::CleanupResources -- vtable slot 3. Releases three shared files plus
 * a four-entry table; it never touches `this`. */
#include "MontyMole.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9d8[];
extern int data_ov080_021283c0[];
extern int data_ov080_021283c8[];
extern int data_ov080_0212766c[];
}

s32 MontyMole::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov080_021283c0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov080_021283c8);
    int i = 0;
    do {
        _ZN13SharedFilePtr7ReleaseEv((void *)data_ov080_0212766c[i]);
        i++;
    } while (i < 4);
    return 1;
}
