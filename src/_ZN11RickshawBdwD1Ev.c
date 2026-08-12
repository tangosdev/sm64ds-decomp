// @symbol _ZN11RickshawBdwD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV11RickshawBdw[];
extern int data_ov002_02109320[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV11RickshawBdw; VT1 = data_ov002_02109320 */
int *_ZN11RickshawBdwD1Ev(int *t)
{
    t[0] = (int)_ZTV11RickshawBdw;
    t[0] = (int)data_ov002_02109320;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
