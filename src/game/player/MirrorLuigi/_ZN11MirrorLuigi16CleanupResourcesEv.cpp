//cpp
// @symbol _ZN11MirrorLuigi16CleanupResourcesEv
/* MirrorLuigi::CleanupResources -- vtable slot 3. Releases the three shared
 * files the class holds; it never touches `this`. */
#include "MirrorLuigi.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210ebb8[];
extern int data_ov002_0210eb20[];
extern int data_ov002_0210eaa0[];
}

int MirrorLuigi::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210ebb8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210eb20);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210eaa0);
    return 1;
}
