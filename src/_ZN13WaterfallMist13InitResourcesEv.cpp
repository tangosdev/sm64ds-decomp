//cpp
// @symbol _ZN13WaterfallMist13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "WaterfallMist.h"
extern signed char data_0209f2f8;

int WaterfallMist::InitResources()
{
    signed char v;
    unk_0d4 = 0x24;
    v = data_0209f2f8;
    if (v != 0x16) {
        if (v == 0x21)
            unk_0d4 = 0xeb;
    } else {
        unk_0d4 = 0x71;
    }
    return 1;
}
