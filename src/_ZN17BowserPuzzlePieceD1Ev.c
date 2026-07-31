// @symbol _ZN17BowserPuzzlePieceD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_MovingCylinderClsn.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV17BowserPuzzlePiece */
extern void func_ov002_020aed18(void *);
int *_ZN17BowserPuzzlePieceD1Ev(int *t)
{
    t[0] = (int)_ZTV17BowserPuzzlePiece;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
