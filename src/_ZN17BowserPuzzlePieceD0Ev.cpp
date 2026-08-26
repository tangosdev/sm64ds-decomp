//cpp
// @symbol _ZN17BowserPuzzlePieceD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "BowserPuzzlePiece.h"

void BowserPuzzlePiece_EmitDeletingDestructor(BowserPuzzlePiece *piece)
{
    delete piece;
}
