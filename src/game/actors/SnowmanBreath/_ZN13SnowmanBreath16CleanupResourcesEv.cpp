//cpp
// @symbol _ZN13SnowmanBreath16CleanupResourcesEv
/* SnowmanBreath::CleanupResources -- vtable slot 3. Releases the three shared
 * files the class holds; it never touches `this`. */
#include "SnowmanBreath.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
}

int SnowmanBreath::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da40);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9c0);
    return 1;
}
