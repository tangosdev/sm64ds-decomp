//cpp
// @symbol _ZN11SnowmanHead16CleanupResourcesEv
/* SnowmanHead::CleanupResources -- vtable slot 3. Releases one shared file, then
 * the two-entry table; it never touches `this`. The legacy C form declared its own
 * `struct SharedFilePtr { int data[4]; }` to type the table; spelled here as void *
 * so the file does not shadow the real class of that name. */
#include "SnowmanHead.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern void *data_ov072_02122bc4;
extern void *data_ov072_02121ffc[];
}

int SnowmanHead::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov072_02122bc4);
    int r5 = 0;
    do {
        _ZN13SharedFilePtr7ReleaseEv(data_ov072_02121ffc[r5]);
        r5++;
    } while (r5 < 2);
    return 1;
}
