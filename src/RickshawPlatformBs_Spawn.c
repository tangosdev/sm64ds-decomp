// @symbol RickshawPlatformBs_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov002_02109278[];
extern int data_ov047_0211244c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02109278 */
int *RickshawPlatformBs_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_02109278;
        p[0] = (int)data_ov047_0211244c;
    }
    return p;
}
