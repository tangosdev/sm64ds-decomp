//cpp
// @symbol _ZN8SignPost16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SignPost.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int SignPost_ClsnFile[];
extern int SignPost_ModelFile[];

int SignPost::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(SignPost_ModelFile))->Release();
    ((SharedFilePtr *)(SignPost_ClsnFile))->Release();
    return 1;
}
