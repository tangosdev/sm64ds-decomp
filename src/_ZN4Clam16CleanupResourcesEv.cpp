//cpp
// @symbol _ZN4Clam16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Three releases, all of this overlay's own handles, and they are the same
 * three InitResources loads: the model at 0211c9c4 and the two animations at
 * 0211c9cc (the lunge) and 0211c9bc (the idle) that Behavior switches
 * between. InitResources loads them animations-first; the release order here
 * is model-first, which is the ROM's and is reproduced verbatim.
 *
 * `SharedFilePtr::Release` is now spelt as the member call it is, so the
 * compiler emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
#include "Clam.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov064_0211c9c4;
extern SharedFilePtr data_ov064_0211c9cc;
extern SharedFilePtr data_ov064_0211c9bc;

int Clam::CleanupResources()
{
    data_ov064_0211c9c4.Release();
    data_ov064_0211c9cc.Release();
    data_ov064_0211c9bc.Release();
    return 1;
}
