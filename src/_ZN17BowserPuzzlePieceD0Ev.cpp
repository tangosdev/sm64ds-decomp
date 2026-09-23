//cpp
// @symbol _ZN17BowserPuzzlePieceD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "BowserPuzzlePiece.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~BowserPuzzlePiece()
 * is defined in the class body, so a qualified call from here would inline
 * a second copy of the whole destructor chain into this arm. The port's D1
 * for this class is its flat ROM name, the name the class's vtable slot 16
 * holds, so this arm makes the D0's two calls: the D1 body through that
 * flat name, then the class-specific operator delete. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN17BowserPuzzlePieceD1Ev(void *self);
extern "C" BowserPuzzlePiece *_ZN17BowserPuzzlePieceD0Ev(BowserPuzzlePiece *thiz)
{
    _ZN17BowserPuzzlePieceD1Ev(thiz);          /* the D1 body, through its flat ROM name */
    BowserPuzzlePiece::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void BowserPuzzlePiece_EmitDeletingDestructor(BowserPuzzlePiece *piece)
{
    delete piece;
}
#endif
