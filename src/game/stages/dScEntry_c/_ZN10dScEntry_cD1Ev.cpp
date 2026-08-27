//cpp
// @symbol _ZN10dScEntry_cD1Ev
#include "dScEntry_c.h"

extern "C" {
void __destroy_arr(void *, int, int, void *);
void func_ov075_02115bac(void *);
void func_ov075_02115bc8(void *);
}

dScEntry_c::~dScEntry_c()
{
    char *self = (char *)this;
    __destroy_arr(self + 0x1b4, 4, 0x2c, (void *)func_ov075_02115bc8);
    __destroy_arr(self + 0x70, 9, 0x24, (void *)func_ov075_02115bac);
}
