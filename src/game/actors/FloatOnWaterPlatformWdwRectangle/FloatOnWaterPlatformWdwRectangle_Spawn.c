// @symbol FloatOnWaterPlatformWdwRectangle_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV17daObjFloatBoard_c[];
extern int _ZTV32FloatOnWaterPlatformWdwRectangle[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjFloatBoard_c */
int *FloatOnWaterPlatformWdwRectangle_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(840);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjFloatBoard_c;
        p[0] = (int)_ZTV32FloatOnWaterPlatformWdwRectangle;
    }
    return p;
}
