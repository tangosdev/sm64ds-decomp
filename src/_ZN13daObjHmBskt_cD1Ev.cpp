//cpp
// @symbol _ZN13daObjHmBskt_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and four destructor calls, all automatic consequences of
 * `struct daObjHmBskt_c : dBgActor_c` and the one member daObjHmBskt_c itself adds:
 * its own vptr, then this class's own dBgCh_Actr at 0x320 -- the only field
 * daObjHmBskt_c declares -- then dBgActor_c's vptr, inlined because dBgActor_c's
 * destructor is defined in its class body, then dBgActor_c's
 * dBgW_KcMbg (0x124) and Model (0xd4) in reverse declaration order,
 * then dActor_c. The empty `{}` body is enough; every one of those calls is
 * the compiler's own member/base cleanup, matching the pre-migration C body's
 * `_ZTV13daObjHmBskt_c; dBgCh_Actr::D1(+0x320); _ZTV10dBgActor_c;
 * dBgW_KcMbg::D1(+0x124); Model::D1(+0xd4); dActor_c::D2` exactly.
 */
#include "daObjHmBskt_c.h"

daObjHmBskt_c::~daObjHmBskt_c()
{
}
