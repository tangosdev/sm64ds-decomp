//cpp
#include "RacingPenguin.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov019_02113498;
extern SharedFilePtr *data_ov019_02112788[7];
extern SharedFilePtr *data_ov019_0211277c[3];
}

int RacingPenguin::CleanupResources()
{
    data_ov019_02113498.Release();
    for (int i = 0; i < 7; i++)
        data_ov019_02112788[i]->Release();
    for (int i = 0; i < 3; i++)
        data_ov019_0211277c[i]->Release();
    return 1;
}
