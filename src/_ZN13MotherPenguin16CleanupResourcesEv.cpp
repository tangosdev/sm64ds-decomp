//cpp
// @symbol _ZN13MotherPenguin16CleanupResourcesEv
#include "MotherPenguin.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov018_02113c00;
extern SharedFilePtr *data_ov018_02112c0c[2];
extern SharedFilePtr *data_ov018_02112c04[2];
}

int MotherPenguin::CleanupResources()
{
    data_ov018_02113c00.Release();
    for (int i = 0; i < 2; i++)
        data_ov018_02112c0c[i]->Release();
    for (int i = 0; i < 2; i++)
        data_ov018_02112c04[i]->Release();
    return 1;
}
