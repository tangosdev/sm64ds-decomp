// @symbol TtcConveyorBeltLarge_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV20TtcConveyorBeltLarge */
int *TtcConveyorBeltLarge_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(928);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV20TtcConveyorBeltLarge;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x334);
    }
    return p;
}
