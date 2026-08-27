//cpp
// @symbol _ZN5Ukiki16CleanupResourcesEv
/* Ukiki::CleanupResources -- vtable slot 3. Releases the four shared files the
 * class holds plus its ten-entry table; it never touches `this`, which is why
 * the legacy C form could declare itself nullary and still reproduce. */
#include "Ukiki.h"

struct SharedFilePtr { u32 data[4]; };

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov002_0210da40;
extern struct SharedFilePtr data_ov002_0210d9a0;
extern struct SharedFilePtr data_ov002_0210d9c0;
extern struct SharedFilePtr data_ov030_02115d00;
extern struct SharedFilePtr *data_ov030_02114824[10];
}

s32 Ukiki::CleanupResources()
{
    int i;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da40);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov030_02115d00);
    for (i = 0; i < 10; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov030_02114824[i]);
    return 1;
}
