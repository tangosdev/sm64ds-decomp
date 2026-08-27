// @symbol CapBlockMario_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13QuestionBlock */
int *CapBlockMario_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13QuestionBlock;
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}
