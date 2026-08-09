//cpp
// @symbol _ZN17BowserPuzzlePiece16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Gives back the two shared files the piece renders from. Both live in ov002,
 * not in this overlay -- the puzzle pieces borrow models the always-resident
 * module owns, so the handles are released rather than freed.
 *
 * `SharedFilePtr::Release` is now spelt as the member call it is, so the
 * compiler emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
#include "BowserPuzzlePiece.h"
#include "SharedFilePtr.h"

extern char data_ov002_0210da10;
extern char data_ov002_0210d9a8;

int BowserPuzzlePiece::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov002_0210da10))->Release();
    ((SharedFilePtr *)(&data_ov002_0210d9a8))->Release();
    return 1;
}
