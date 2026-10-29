//cpp
#include "CP15.h"

extern "C" {

extern unsigned short data_020a89a8;
extern unsigned int *data_020a89ac;

int func_0206152c(void) {
    return data_020a89a8 ? 0 : 3;
}

int func_020614d0(void) {
    int r = func_0206152c();
    if (r != 0)
        return r;
    CP15::InvalidateDataCache(data_020a89ac[1], 2);
    return *(unsigned short *)data_020a89ac[1] <= 1 ? 3 : 0;
}
}
