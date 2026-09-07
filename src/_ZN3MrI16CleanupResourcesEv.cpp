//cpp
#include "MrI.h"
#include "SharedFilePtr.h"

extern "C" {
void UnloadBlueCoinModel(void);
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov071_02123050;
extern SharedFilePtr *data_ov071_021226a4[2];
extern SharedFilePtr *data_ov071_021226a0;
}

s32 MrI::CleanupResources()
{
    UnloadBlueCoinModel();
    data_ov002_0210da38.Release();
    data_ov071_02123050.Release();
    for (s32 i = 0; i < 2; ++i) {
        data_ov071_021226a4[i]->Release();
    }
    data_ov071_021226a0->Release();
    return 1;
}
