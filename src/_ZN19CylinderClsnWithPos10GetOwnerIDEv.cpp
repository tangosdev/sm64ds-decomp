//cpp
// @symbol _ZN19CylinderClsnWithPos10GetOwnerIDEv
/* recovered: named members + shared header, real C++ method */
#include "CylinderClsnWithPos.h"
/* CylinderClsnWithPos::GetOwnerID() at 0x02014818
 * vtable slot 3. A positional cylinder has no owning Actor, so its owner ID
 * is always 0.
 */

typedef unsigned int u32;

struct CylinderClsnWithPos;

u32 CylinderClsnWithPos::GetOwnerID()
{
    (void)this;
    return 0;
}
