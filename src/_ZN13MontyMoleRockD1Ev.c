// @symbol _ZN13MontyMoleRockD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13MontyMoleRock */
extern void _ZN5EnemyD2Ev(void *);
int *_ZN13MontyMoleRockD1Ev(int *t)
{
    t[0] = (int)_ZTV13MontyMoleRock;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x194);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x160);
    _ZN5ModelD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
