//cpp
// @symbol _ZN5Model13GetVramOffsetEj
#include "Model.h"
extern "C" {
extern u32 data_020a4bc8;
extern u32 data_020a4bdc;
extern u32 data_020a4be0;
extern u32 data_020a4be8;
void Crash();
}

u32 Model::GetVramOffset(u32 size)
{
    u32 *p;
    u32 b = data_020a4be8 - data_020a4bc8;
    u32 a = data_020a4bdc - data_020a4be0;
    if ((a << 1) > b && a > size) {
        p = &data_020a4bdc;
    } else if (b > size) {
        p = &data_020a4be8;
    } else {
        Crash();
    }
    *p -= size;
    return *p;
}
