//cpp
// @symbol _ZN9Butterfly16CleanupResourcesEv
/* Butterfly::CleanupResources -- vtable slot 3. Releases the four shared files
 * the class holds -- three of its own in ov100 and one shared with the other
 * small fauna in ov002; it never touches `this`. */
#include "Butterfly.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov100_02148608[];
extern int data_ov100_02148600[];
extern int data_ov002_0210d9d8[];
extern int data_ov100_02148668[];
}

int Butterfly::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_02148608);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_02148600);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_02148668);
    return 1;
}
