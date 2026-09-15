//cpp
// @symbol _ZN11dCapEnemy_c14UnloadCapModelEv
/* recovered: named members + shared header, real C++ method
 *
 * Drops this enemy's claim on its cap model. mCapId is masked with & 7 first,
 * because ReleaseCap ORs marker bits into the top of the same byte -- and the
 * >= 6 test then covers both "no cap" and any index the table cannot serve.
 */
#include "dCapEnemy_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern struct SharedFilePtr *data_ov002_020ff028[6];
}

void dCapEnemy_c::UnloadCapModel()
{
    s32 idx = mCapId & 7;
    if (idx >= 6) return;
    data_ov002_020ff028[idx]->Release();
}
