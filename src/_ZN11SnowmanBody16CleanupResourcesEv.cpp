//cpp
// @symbol _ZN11SnowmanBody16CleanupResourcesEv
/* SnowmanBody::CleanupResources -- vtable slot 3. Releases the one shared file
 * the class holds; it never touches `this`. */
#include "SnowmanBody.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov072_02122b20[];
}

int SnowmanBody::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122b20);
    return 1;
}
