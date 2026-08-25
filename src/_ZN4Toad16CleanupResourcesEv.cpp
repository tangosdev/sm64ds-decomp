//cpp
// @symbol _ZN4Toad16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Toad.h"
#include "SharedFilePtr.h"
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern int data_ov085_02130480[];

int Toad::CleanupResources()
{
    if (*(unsigned char *)((char *)&mVariant) == 1) {
        ((SharedFilePtr *)(data_ov002_0210da40))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9a0))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9c0))->Release();
    }
    ((SharedFilePtr *)(data_ov085_02130480))->Release();
    ((SharedFilePtr *)(data_ov085_02130488))->Release();
    ((SharedFilePtr *)(data_ov085_02130490))->Release();
    return 1;
}
