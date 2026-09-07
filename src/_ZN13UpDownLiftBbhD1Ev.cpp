//cpp
// @symbol _ZN13UpDownLiftBbhD1Ev
/* recovered: real C++ destructor -- the compiler emits the entire teardown
 *
 * UpDownLiftBbh owns no class-typed members of its own. Its empty destructor
 * changes to the derived vtable, then the inline dBgActor_c destructor changes
 * to the base vtable and destroys the moving collider, model and actor base.
 */
#include "UpDownLiftBbh.h"

UpDownLiftBbh::~UpDownLiftBbh()
{
}
