// @symbol _ZN10ChillBullyD0Ev
/* ChillBully's deleting destructor -- vtable SLOT 17, previously
 * func_ov027_02111618.
 *
 * It used to carry the recovered name `daIDonketu_c::OnYoshiTryEat`, which was
 * off by one slot: OnYoshiTryEat is slot 18 and this class does not override it
 * (slot 18 is still _ZN5Actor13OnYoshiTryEatEv). The body settles it anyway -- it
 * tears the object down and then calls Memory::Deallocate, which no eat handler does.
 */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int _ZTV10ChillBully[];
extern int _ZTV7daOts_c[];
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN10ChillBullyD0Ev(int *t)
{
    t[0] = (int)_ZTV10ChillBully;
    t[0] = (int)_ZTV7daOts_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x33c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
