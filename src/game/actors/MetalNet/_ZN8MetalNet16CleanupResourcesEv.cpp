//cpp
// @symbol _ZN8MetalNet16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MetalNet.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov009_02113e88[];
extern int data_ov009_02113e90[];

int MetalNet::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov009_02113e90))->Release();
    ((SharedFilePtr *)(data_ov009_02113e88))->Release();
    return 1;
}
