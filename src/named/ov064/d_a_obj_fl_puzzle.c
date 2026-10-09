// @symbol daObjFl_Puzzle_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV16daObjFl_Puzzle_c */
/* Reconstructed source-style name: SM64DS proves daObjFl_Puzzle_c through RTTI,
 * allocation size, vtable identity, and the FL_PUZZLE registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The implementation was earlier named BowserPuzzlePiece.
 * Historical alias: BowserPuzzlePiece_Spawn. */
int *daObjFl_Puzzle_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(828);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV16daObjFl_Puzzle_c; }
    return p;
}
