// @symbol _ZN13MontyMoleRockD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV14daChoro_Rock_c */
extern void func_ov002_020aed18(void *);
extern void *G0;
int *_ZN13MontyMoleRockD0Ev(int *t)
{
    t[0] = (int)_ZTV14daChoro_Rock_c;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x194);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x160);
    _ZN5ModelD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
