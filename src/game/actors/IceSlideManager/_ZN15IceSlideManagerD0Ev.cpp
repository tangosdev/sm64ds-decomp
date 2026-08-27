//cpp
// @symbol _ZN15IceSlideManagerD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "IceSlideManager.h"

void IceSlideManager_EmitDeletingDestructor(IceSlideManager *manager)
{
    delete manager;
}
