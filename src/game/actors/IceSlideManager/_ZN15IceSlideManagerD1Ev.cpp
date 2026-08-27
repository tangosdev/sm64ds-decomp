//cpp
// @symbol _ZN15IceSlideManagerD1Ev
/* Force mwccarm to materialize IceSlideManager's inline complete destructor. */
#include "IceSlideManager.h"

void IceSlideManager_EmitDestructor(IceSlideManager *manager)
{
    manager->~IceSlideManager();
}
