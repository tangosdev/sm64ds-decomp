//cpp
// @symbol _ZN13TreasureChest8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TreasureChest.h"
struct CylinderClsn { int dummy; };
extern void _ZN12CylinderClsn5ClearEv(struct CylinderClsn *t);
extern void _ZN12CylinderClsn6UpdateEv(struct CylinderClsn *t);

int TreasureChest::Behavior()
{
    func_ov064_0211a734(((char *)this));
    _ZN12CylinderClsn5ClearEv((struct CylinderClsn *)((char *)&mMovingCylinderClsn));
    _ZN12CylinderClsn6UpdateEv((struct CylinderClsn *)((char *)&mMovingCylinderClsn));
    return 1;
}
