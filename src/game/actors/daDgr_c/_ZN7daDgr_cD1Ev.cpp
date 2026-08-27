//cpp
// @symbol _ZN7daDgr_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and two destructor calls, every one a consequence of
 * `struct daDgr_c : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class adds
 * no member with a destructor of its own; its own 0x14 tail bytes are POD.
 */
#include "daDgr_c.h"

daDgr_c::~daDgr_c()
{
}
