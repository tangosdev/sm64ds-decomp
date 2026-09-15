//cpp
// @symbol _ZN9MontyMole16CleanupResourcesEv
/* MontyMole::CleanupResources -- vtable slot 3. Releases three shared files plus
 * a four-entry table; it never touches `this`. */
#include "MontyMole.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov080_021283c0;
extern SharedFilePtr data_ov080_021283c8;
extern SharedFilePtr *data_ov080_0212766c[];
}

s32 MontyMole::CleanupResources()
{
    data_ov002_0210d9d8.Release();
    data_ov080_021283c0.Release();
    data_ov080_021283c8.Release();
    int i = 0;
    do {
        data_ov080_0212766c[i]->Release();
        i++;
    } while (i < 4);
    return 1;
}
