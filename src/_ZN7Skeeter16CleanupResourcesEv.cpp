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
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov090_021344a0;
extern SharedFilePtr data_ov090_02134488;
extern SharedFilePtr data_ov090_02134480;
extern SharedFilePtr data_ov090_02134490;
extern SharedFilePtr data_ov090_02134498;
}

int Skeeter::CleanupResources()
{
    data_ov090_021344a0.Release();
    data_ov090_02134488.Release();
    data_ov090_02134480.Release();
    data_ov090_02134490.Release();
    data_ov090_02134498.Release();
    return 1;
}
