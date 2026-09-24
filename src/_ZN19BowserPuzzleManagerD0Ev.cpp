//cpp
// @symbol _ZN19BowserPuzzleManagerD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~BowserPuzzleManager()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "BowserPuzzleManager.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single
 * ??1BowserPuzzleManager@@QAE@XZ that src/_ZN19BowserPuzzleManagerD1Ev.cpp
 * defines out of line (include/BowserPuzzleManager.h declares
 * ~BowserPuzzleManager() with no slot under _MSC_VER), so this file cannot
 * carry the same definition: the host link refuses the pair (LNK2005). It
 * spells out, in terms of that one host symbol, what the variant this file is
 * enrolled for does: the D1 body, then the class-specific operator delete
 * (Memory::Deallocate with the game heap, the word at 0x020a0eac, from
 * dActor_c's inline operator delete). The qualified call is direct. The port's
 * slot 17 for this class (shl_d0 in hal/actor_classes_bowserpuzzle.cpp) calls
 * this name. Nothing here reaches mwccarm: it builds the `#else` arm and emits
 * the ROM bytes it always emitted, and the object is byte-identical either
 * way. */
extern "C" BowserPuzzleManager *_ZN19BowserPuzzleManagerD0Ev(BowserPuzzleManager *thiz)
{
    thiz->BowserPuzzleManager::~BowserPuzzleManager();          /* the D1 body, through the one host symbol */
    BowserPuzzleManager::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
BowserPuzzleManager::~BowserPuzzleManager()
{
}
#endif
