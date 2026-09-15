//cpp
// @symbol _ZN11SnowmanHead16CleanupResourcesEv
/* SnowmanHead::CleanupResources -- vtable slot 3. Releases one shared file, then
 * the two-entry table; it never touches `this`. The legacy C form declared its own
 * `struct SharedFilePtr { int data[4]; }` to type the table; spelled here as void *
 * so the file does not shadow the real class of that name. */
#include "SnowmanHead.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov072_02122bc4;
extern SharedFilePtr *data_ov072_02121ffc[];
}

int SnowmanHead::CleanupResources()
{
    data_ov072_02122bc4.Release();
    int r5 = 0;
    do {
        data_ov072_02121ffc[r5]->Release();
        r5++;
    } while (r5 < 2);
    return 1;
}
