// @symbol _ZN9RabbitKeyD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9RabbitKey */
extern void _ZN5EnemyD2Ev(void *);
int *_ZN9RabbitKeyD1Ev(int *t)
{
    t[0] = (int)_ZTV9RabbitKey;
    _ZN11ShadowModelD1Ev((char *)t + 0x160);
    _ZN5ModelD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
