//cpp
// @symbol _ZN6EyerokD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Own vptr, then the members in reverse declaration order (the class-owned
 * dBgW_KcMbg at 0x674, the Vector3 span at 0x4dc via __destroy_arr,
 * TextureSequence, ShadowModel, Model, BlendModelAnim,
 * dCcAcPos_c), then dBgActor_c's vptr -- inlined, because
 * dBgActor_c's destructor is defined in its class body -- then dBgActor_c's
 * own dBgW_Kc and Model, then dActor_c. See include/Eyerok.h for why
 * this is a direct dBgActor_c child, not three levels deep.
 */
#include "Eyerok.h"

Eyerok::~Eyerok()
{
}
