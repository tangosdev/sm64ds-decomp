//cpp
// @symbol _ZN15daObjMarioCap_c16OnPendingDestroyEv

#include "daObjMarioCap_c.h"

void daObjMarioCap_c::OnPendingDestroy()
{
    if (unk_400 == 0xff)
        return;
    mCapIcon.Unlink();
}
