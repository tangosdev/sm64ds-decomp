// @symbol FloatOnWaterPlatformWdwRectangle_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov029_02113f44[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV14daObjWcObj06_c */
int *FloatOnWaterPlatformWdwRectangle_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(840);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV14daObjWcObj06_c;
        p[0] = (int)data_ov029_02113f44;
    }
    return p;
}
