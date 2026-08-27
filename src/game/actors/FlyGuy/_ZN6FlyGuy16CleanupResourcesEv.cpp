//cpp
// @symbol _ZN6FlyGuy16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 7 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "FlyGuy.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov070_02123530[];
extern int data_ov070_02123520[];
extern int data_ov070_02123518[];
extern int data_ov070_02123510[];
extern int data_ov070_02123528[];
extern int data_ov070_02123508[];
extern int data_ov070_02123500[];
}

int FlyGuy::CleanupResources()
{
    ((SharedFilePtr *)data_ov070_02123530)->Release();
    ((SharedFilePtr *)data_ov070_02123520)->Release();
    ((SharedFilePtr *)data_ov070_02123518)->Release();
    ((SharedFilePtr *)data_ov070_02123510)->Release();
    ((SharedFilePtr *)data_ov070_02123528)->Release();
    ((SharedFilePtr *)data_ov070_02123508)->Release();
    ((SharedFilePtr *)data_ov070_02123500)->Release();
    return 1;
}
