// @symbol func_ov045_02111b14
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov045_02112f50[];
extern int data_ov002_0210912c[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov045_02112f50; VT1 = data_ov002_0210912c */
int *func_ov045_02111b14(int *t)
{
    t[0] = (int)data_ov045_02112f50;
    t[0] = (int)data_ov002_0210912c;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
