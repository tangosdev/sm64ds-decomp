// @symbol func_ov065_0211a45c
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov065_0211d0ec[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov065_0211d0ec */
int *func_ov065_0211a45c(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(904);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov065_0211d0ec;
        _ZN11ShadowModelC1Ev((char *)p + 0x330);
    }
    return p;
}
