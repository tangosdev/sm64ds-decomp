//cpp
// @symbol _ZN11BabyPenguin16CleanupResourcesEv

#include "BabyPenguin.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov072_02122cb4;
extern SharedFilePtr *data_ov072_02122004[];

int BabyPenguin::CleanupResources()
{
    data_ov072_02122cb4.Release();

    int i = 0;
    do {
        data_ov072_02122004[i]->Release();
        i++;
    } while (i < 5);

    return 1;
}
