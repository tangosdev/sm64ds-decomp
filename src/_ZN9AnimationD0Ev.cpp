//cpp
// @symbol _ZN9AnimationD0Ev
/* The compiler emits the deleting destructor from this ordinary C++
 * destructor definition. Animation::operator delete supplies the ROM's
 * Memory::operator_delete2 destination; objisolate retains D0 here.
 */
#include "Animation.h"

Animation::~Animation()
{
}
