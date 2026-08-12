// @symbol func_ov043_021113fc
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov043_0211238c[];
extern int data_ov002_02109320[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov043_0211238c; VT1 = data_ov002_02109320 */
int *func_ov043_021113fc(int *t)
{
    t[0] = (int)data_ov043_0211238c;
    t[0] = (int)data_ov002_02109320;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
