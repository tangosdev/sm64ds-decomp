// @symbol ShutterHmc_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov002_021099e4[];
extern int _ZTV10ShutterHmc[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_021099e4 */
int *ShutterHmc_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(804);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_021099e4;
        p[0] = (int)_ZTV10ShutterHmc;
    }
    return p;
}
