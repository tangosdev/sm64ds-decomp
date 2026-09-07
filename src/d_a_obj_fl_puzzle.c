// @symbol daObjFl_Puzzle_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV17BowserPuzzlePiece */
/* Reconstructed source-style name: SM64DS proves daObjFl_Puzzle_c through RTTI,
 * allocation size, vtable identity, and the FL_PUZZLE registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's BowserPuzzlePiece implementation aliases remain unchanged.
 * Historical alias: BowserPuzzlePiece_Spawn. */
int *daObjFl_Puzzle_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(828);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV17BowserPuzzlePiece; }
    return p;
}
