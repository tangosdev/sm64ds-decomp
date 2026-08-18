//cpp
// @symbol _ZN3Amp16CleanupResourcesEv

#include "Amp.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov070_021235fc;
extern SharedFilePtr data_ov070_02123604;
extern SharedFilePtr *data_ov070_021222e0[2];
extern SharedFilePtr data_ov070_021235ec;

int Amp::CleanupResources()
{
    data_ov070_021235fc.Release();
    data_ov070_02123604.Release();

    int i = 0;
    do {
        data_ov070_021222e0[i]->Release();
        i++;
    } while (i < 2);

    data_ov070_021235ec.Release();
    return 1;
}
