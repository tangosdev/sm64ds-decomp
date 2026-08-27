//cpp
// @symbol _ZN7HeaveHo16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 4 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "HeaveHo.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov077_02127c88[];
extern int data_ov077_02127ca0[];
extern int data_ov077_02127c90[];
extern int data_ov077_02127c98[];
}

int HeaveHo::CleanupResources()
{
    ((SharedFilePtr *)data_ov077_02127c88)->Release();
    ((SharedFilePtr *)data_ov077_02127ca0)->Release();
    ((SharedFilePtr *)data_ov077_02127c90)->Release();
    ((SharedFilePtr *)data_ov077_02127c98)->Release();
    return 1;
}
