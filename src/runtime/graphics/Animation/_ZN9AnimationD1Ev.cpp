//cpp
// @symbol _ZN9AnimationD1Ev
/* D1, the complete-object destructor. One `Animation::~Animation()` definition makes
 * mwcc emit D0, D1 and D2 together; objisolate keeps the one this file is
 * bound to by its delinks entry, so the D0/D2 siblings carry the same
 * definition in their own files. */
#include "Animation.h"

Animation::~Animation()
{
}
