// @symbol FloatOnWaterPlatformWdwRectangle_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov002_02108fdc[];
extern int _ZTV32FloatOnWaterPlatformWdwRectangle[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02108fdc */
int *FloatOnWaterPlatformWdwRectangle_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(840);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_02108fdc;
        p[0] = (int)_ZTV32FloatOnWaterPlatformWdwRectangle;
    }
    return p;
}
