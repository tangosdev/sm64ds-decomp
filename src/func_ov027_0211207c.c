// @symbol func_ov027_0211207c
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_Platform.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV10daPgDfdr_c */
int *func_ov027_0211207c(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(988);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV10daPgDfdr_c;
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN15TextureSequenceC1Ev((char *)p + 0x384);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x398);
    }
    return p;
}
