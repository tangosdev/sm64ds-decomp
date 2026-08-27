//cpp
// @symbol _ZN15daObjMarioCap_c16OnPendingDestroyEv

#include "daObjMarioCap_c.h"

extern "C" void func_ov001_020ab110(CapIcon *icon);

void daObjMarioCap_c::OnPendingDestroy()
{
    if (unk_400 == 0xff)
        return;
    func_ov001_020ab110(&mCapIcon);
}
