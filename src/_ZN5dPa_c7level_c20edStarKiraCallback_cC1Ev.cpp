//cpp
// @symbol _ZN5dPa_c7level_c20edStarKiraCallback_cC1Ev
#include "dPa_c.h"

/* This initializer's 0xc-byte layout is evidenced, but its original class
 * name is not. Keep that single unresolved ABI boundary explicit. */
extern "C" void func_0204dc84(char* record);

dPa_c::level_c::edStarKiraCallback_c::edStarKiraCallback_c()
{
    char* record = (char*)trackingRecords;
    int i = 0;
    do {
        func_0204dc84(record);
        ++i;
        record += 0xc;
    } while (i < 0x40);

    unk308 = 0x1000;
    unk30c = 0x200;
}
