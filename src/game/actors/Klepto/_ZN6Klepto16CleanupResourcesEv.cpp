//cpp
// @symbol _ZN6Klepto16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the seven shared files InitResources claimed -- three of them from
 * ov002, which is where Klepto's shared Mario-cap assets live.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Klepto.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern int data_ov062_0211e0fc[];
extern int data_ov062_0211e114[];
extern int data_ov062_0211e10c[];
extern int data_ov062_0211e104[];
}

int Klepto::CleanupResources()
{
    ((SharedFilePtr *)data_ov002_0210da40)->Release();
    ((SharedFilePtr *)data_ov002_0210d9a0)->Release();
    ((SharedFilePtr *)data_ov002_0210d9c0)->Release();
    ((SharedFilePtr *)data_ov062_0211e0fc)->Release();
    ((SharedFilePtr *)data_ov062_0211e114)->Release();
    ((SharedFilePtr *)data_ov062_0211e10c)->Release();
    ((SharedFilePtr *)data_ov062_0211e104)->Release();
    return 1;
}
