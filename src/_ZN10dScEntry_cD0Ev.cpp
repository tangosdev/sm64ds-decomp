//cpp
// @symbol _ZN10dScEntry_cD0Ev
/* Deleting variant of the same compiler-owned icon-array teardown documented
   by _ZN10dScEntry_cD1Ev.cpp. */
#include "dScEntry_c.h"

extern "C" {
void __destroy_arr(void *, int, int, void *);
void func_ov075_02115bc8(void *);
}

dScEntry_c::~dScEntry_c()
{
    __destroy_arr(mUnkArray, 4, sizeof(mUnkArray[0]),
                  (void *)func_ov075_02115bc8);
}
