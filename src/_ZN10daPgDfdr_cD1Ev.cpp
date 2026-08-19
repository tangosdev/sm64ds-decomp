//cpp
// @symbol _ZN10daPgDfdr_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Own vtable store, then (in reverse declaration order) dCcAc_c's
 * destructor, TextureSequence's, ModelAnim's -- the three members this class
 * declares -- then dBgActor_c's vtable store (inlined, since dBgActor_c's own
 * destructor is defined in its class body), then dBgActor_c's dBgW_KcMbg
 * and Model, then dActor_c. Every one of those is a consequence of
 * `struct daPgDfdr_c : dBgActor_c` with three real class-typed members and no
 * hand-written body of its own.
 */
#include "daPgDfdr_c.h"

daPgDfdr_c::~daPgDfdr_c()
{
}
