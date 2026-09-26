//cpp
// @symbol _ZN17BowserPuzzlePieceD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "BowserPuzzlePiece.h"

#ifdef _MSC_VER
/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" BowserPuzzlePiece *_ZN17BowserPuzzlePieceD0Ev(BowserPuzzlePiece *thiz)
{
    thiz->BowserPuzzlePiece::~BowserPuzzlePiece();          /* direct member/base teardown */
    BowserPuzzlePiece::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void BowserPuzzlePiece_EmitDeletingDestructor(BowserPuzzlePiece *piece)
{
    delete piece;
}
#endif
