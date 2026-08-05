// @symbol RotatingPlatformLll_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov002_021091d4[];
extern int data_ov022_02113de8[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_021091d4 */
int *RotatingPlatformLll_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_021091d4;
        p[0] = (int)data_ov022_02113de8;
    }
    return p;
}
