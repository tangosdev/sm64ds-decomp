//cpp
// @symbol _ZN11ShadowModel8CleanAllEv
#include "ShadowModel.h"
extern "C" {
extern u8 data_0209ceec;
extern ShadowModel *data_0209cef4;
}

void ShadowModel::CleanAll()
{
    if (data_0209cef4) {
        do {
            ShadowModel *nx = data_0209cef4->next;
            if (nx)
                nx->prev = 0;
            /* the ROM re-reads the head for this store instead of keeping
               it in a register */
            data_0209cef4->next = 0;
            data_0209cef4 = nx;
        } while (data_0209cef4);
    }
    data_0209ceec = 0;
}
