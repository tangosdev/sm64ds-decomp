//cpp
// @symbol _ZN7Chuckya16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the six shared files InitResources claimed: one model and five
 * animations. Order is not the claim order -- it releases the model first,
 * then the animations in their load order -- but every one of the six is
 * paired, which is the property that matters.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Chuckya.h"

extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov062_0211ddf0[];
extern int data_ov062_0211dde8[];
extern int data_ov062_0211dde0[];
extern int data_ov062_0211de00[];
extern int data_ov062_0211de08[];
extern int data_ov062_0211ddf8[];
}

int Chuckya::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211ddf0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211dde8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211dde0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211de00);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211de08);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211ddf8);
    return 1;
}
