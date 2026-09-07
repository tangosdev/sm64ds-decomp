//cpp
// @symbol _ZN7Skeeter16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the five shared files InitResources claimed -- one model and four
 * animations. Not in claim order, but every one is paired.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Skeeter.h"

extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov090_021344a0[];
extern int data_ov090_02134488[];
extern int data_ov090_02134480[];
extern int data_ov090_02134490[];
extern int data_ov090_02134498[];
}

int Skeeter::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_021344a0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134488);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134480);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134490);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134498);
    return 1;
}
