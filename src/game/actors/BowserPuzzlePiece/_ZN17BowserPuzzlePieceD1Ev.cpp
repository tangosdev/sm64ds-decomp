//cpp
// @symbol _ZN17BowserPuzzlePieceD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "BowserPuzzlePiece.h"

void BowserPuzzlePiece_EmitDestructor(BowserPuzzlePiece *piece)
{
    piece->~BowserPuzzlePiece();
}
