//cpp
#include "CP15.h"

extern "C" {

extern unsigned int *data_020a7fc0;

unsigned int func_0205b5d4(void) {
    CP15::InvalidateDataCache((unsigned int)data_020a7fc0, 0x280);
    return data_020a7fc0[0];
}
}
