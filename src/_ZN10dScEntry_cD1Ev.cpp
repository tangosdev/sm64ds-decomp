//cpp
// @symbol _ZN10dScEntry_cD1Ev
/* The unidentified 0x1b4 array retains its explicit no-op element callback.
   After this body, mwccarm automatically destroys the typed mIcons array in
   reverse order and then chains through dScene_c, reproducing the ROM. */
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
