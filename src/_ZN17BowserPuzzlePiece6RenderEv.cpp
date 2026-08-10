//cpp
// @symbol _ZN17BowserPuzzlePiece6RenderEv
/* recovered: shared header, real C++ method
 *
 * `return 1` and nothing else -- the whole ROM body is `mov r0,#1; bx lr`.
 * The piece draws nothing of its own from the render slot; whatever puts it
 * on screen is not this override.
 */
#include "BowserPuzzlePiece.h"

s32 BowserPuzzlePiece::Render()
{
    return 1;
}
