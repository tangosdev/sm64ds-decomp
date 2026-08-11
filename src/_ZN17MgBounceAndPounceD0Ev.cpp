//cpp
// @symbol _ZN17MgBounceAndPounceD0Ev
/* Forces the deleting destructor out-of-line. Same reasoning as
   _ZN17MgBounceAndPounceD1Ev.cpp. */
#include "MgBounceAndPounce.h"
void MgBounceAndPounce_EmitDeletingDestructor(MgBounceAndPounce *p)
{
    delete p;
}
