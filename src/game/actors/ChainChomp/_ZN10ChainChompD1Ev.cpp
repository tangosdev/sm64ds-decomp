//cpp
// @symbol _ZN10ChainChompD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Four array cleanups and three member destructors, reverse declaration order
 * out of ChainChomp.h. Seven links means seven of nearly everything: Model[7]
 * at 0x1dc, ShadowModel[7] at 0x40c, and two Vector3[7] at 0x524 and 0x578 for
 * the per-link positions. Every element type was already named in the tree.
 */
#include "ChainChomp.h"

ChainChomp::~ChainChomp()
{
}
