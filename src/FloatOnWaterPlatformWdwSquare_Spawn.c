// @symbol FloatOnWaterPlatformWdwSquare_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV17daObjFloatBoard_c[];
extern int _ZTV29FloatOnWaterPlatformWdwSquare[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjFloatBoard_c */
int *FloatOnWaterPlatformWdwSquare_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(840);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV17daObjFloatBoard_c;
        p[0] = (int)_ZTV29FloatOnWaterPlatformWdwSquare;
    }
    return p;
}
