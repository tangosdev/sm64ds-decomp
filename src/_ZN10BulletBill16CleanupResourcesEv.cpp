//cpp
// @symbol _ZN10BulletBill16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the two model files InitResources claimed -- one per Model member.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "BulletBill.h"

extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov079_02128300[];
extern int data_ov079_021282f0[];
}

int BulletBill::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov079_02128300);
    _ZN13SharedFilePtr7ReleaseEv(data_ov079_021282f0);
    return 1;
}
