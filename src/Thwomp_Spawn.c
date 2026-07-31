// @symbol Thwomp_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV7daDsn_c */
int *Thwomp_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(932);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV7daDsn_c;
        _ZN15TextureSequenceC1Ev((char *)p + 0x324);
        _ZN11ShadowModelC1Ev((char *)p + 0x338);
        p[0] = (int)VT1;
    }
    return p;
}
