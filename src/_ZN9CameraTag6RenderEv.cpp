//cpp
// @symbol _ZN13InvisiblePole6RenderEv
/* recovered: shared header, real C++ method
 *
 * `return 1` -- which is what makes it INVISIBLE. There is no model to draw,
 * and the override exists so the base never tries.
 */
#include "InvisiblePole.h"

int InvisiblePole::Render()
{
    return 1;
}
