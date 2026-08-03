//cpp
// @symbol _ZN5Bully16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Bully.h"
#include "SharedFilePtr.h"
extern "C" {
}

int Bully::CleanupResources()
{
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+4)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+8)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0xc)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0x10)))->Release();
    return 1;
}
