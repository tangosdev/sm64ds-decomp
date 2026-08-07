// @symbol _ZN9SubmarineD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9Submarine */
extern void _ZN5EnemyD2Ev(void *);
int *_ZN9SubmarineD1Ev(int *t)
{
    t[0] = (int)_ZTV9Submarine;
    _ZN18TextureTransformerD1Ev((char *)t + 0x178);
    _ZN9ModelAnimD1Ev((char *)t + 0x114);
    _ZN5EnemyD2Ev(t);
    return t;
}
