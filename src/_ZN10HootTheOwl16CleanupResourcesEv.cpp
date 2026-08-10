//cpp
// @symbol _ZN10HootTheOwl16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the four files InitResources claimed -- the model, and the three
 * animations it hands to Animation::LoadFile.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "HootTheOwl.h"

extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov094_02136ae0[];
extern int data_ov094_02136af8[];
extern int data_ov094_02136ae8[];
extern int data_ov094_02136af0[];
}

int HootTheOwl::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136ae0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136af8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136ae8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136af0);
    return 1;
}
