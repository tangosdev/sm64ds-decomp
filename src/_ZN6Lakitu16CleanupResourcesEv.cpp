//cpp
// @symbol _ZN6Lakitu16CleanupResourcesEv

#include "Lakitu.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov077_02127b50;
extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr *data_ov077_02127238[2];
extern SharedFilePtr *data_ov077_02127230[2];

int Lakitu::CleanupResources()
{
    data_ov077_02127b50.Release();
    data_ov077_02127b48.Release();
    data_ov077_02127b38.Release();

    for (int i = 0; i < 2; i++)
        data_ov077_02127238[i]->Release();

    for (int i = 0; i < 2; i++)
        data_ov077_02127230[i]->Release();

    return 1;
}
