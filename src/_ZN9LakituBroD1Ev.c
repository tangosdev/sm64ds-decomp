// @symbol _ZN9LakituBroD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9LakituBro */
extern void _ZN15TextureSequenceD1Ev(void *);
extern void func_ov002_020aed18(void *);
int *_ZN9LakituBroD1Ev(int *t)
{
    t[0] = (int)_ZTV9LakituBro;
    _ZN11ShadowModelD1Ev((char *)t + 0x218);
    _ZN11ShadowModelD1Ev((char *)t + 0x1f0);
    _ZN15TextureSequenceD1Ev((char *)t + 0x1d8);
    _ZN9ModelAnimD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
