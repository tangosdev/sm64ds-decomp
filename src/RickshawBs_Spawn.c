// @symbol RickshawBs_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov002_02109320[];
extern int data_ov047_021122a0[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02109320 */
int *RickshawBs_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(816);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_02109320;
        p[0] = (int)data_ov047_021122a0;
    }
    return p;
}
