//cpp
// @symbol _ZN11ShadowModelD0Ev
/* recovered: the deleting ABI variant emitted from the real C++ destructor
 *
 * The source body is intentionally the same as D1. mwccarm supplies the vptr
 * reset, ModelBase subobject teardown and Memory::operator delete wrapper that
 * distinguish D0 from D1; the 2004 EAD source only had this destructor body.
 */
#include "ShadowModel.h"

extern ShadowModel *data_0209cef4;  /* head of the live-shadow list */

ShadowModel::~ShadowModel()
{
    if (prev)
        prev->next = next;
    else if (data_0209cef4 == this)
        data_0209cef4 = next;

    if (next)
        next->prev = prev;

    prev = 0;
    next = 0;
}
