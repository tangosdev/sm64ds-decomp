//cpp
// @symbol _ZN11MirrorLuigiD0Ev
/* Real compiler-spelled deleting destructor. The same class definition drives
 * D1 and D0; dActor_c's inline operator delete supplies the actor-heap release
 * after the complete MirrorLuigi destruction sequence. */
#include "MirrorLuigi.h"

MirrorLuigi::~MirrorLuigi()
{
}
