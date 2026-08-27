//cpp
// @symbol _ZN7daDkk_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and five destructor calls, every one a consequence of
 * `struct daDkk_c : daDsnBase_c`: its own vptr, then daDsnBase_c's --
 * inlined, because daDsnBase_c's destructor is defined in its class body --
 * which itself destroys ShadowModel@0x338 then TextureSequence@0x324, then
 * dBgActor_c's -- also inlined, per its own header's convention -- which
 * destroys dBgW_KcMbg@0x124 and Model@0xd4, then dActor_c. This
 * class adds no member of its own with a destructor.
 */
#include "daDkk_c.h"

daDkk_c::~daDkk_c()
{
}
