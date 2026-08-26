//cpp
// @symbol _ZN4TreeD1Ev
/* recovered: real C++ complete-object destructor -- the compiler emits the
 * whole body. An empty ~Tree() stores the class vtable, destroys mModel[5]
 * through __destroy_arr, and runs the dActor_c base step; the deleting
 * variant gets its heap tail from dActor_c's inline operator delete.
 *
 * Replaces the hand-written extern "C" definition kept by the #1719-era
 * "no conversion benefit" note; with mModel now a real typed member array
 * there is nothing left to spell by hand.
 */
#include "Tree.h"

Tree::~Tree()
{
}
