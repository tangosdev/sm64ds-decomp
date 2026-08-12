//cpp
// @symbol _ZN8YoshiEggD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and five destructor calls, every one a consequence of
 * `struct YoshiEgg : Enemy` and the four members that declaration types, destroyed in
 * reverse declaration order, then Enemy. This class adds no member with a destructor
 * of its own -- a Player pointer and three scalars.
 */
#include "YoshiEgg.h"

YoshiEgg::~YoshiEgg()
{
}
