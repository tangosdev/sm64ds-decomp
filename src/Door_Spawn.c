// @symbol Door_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int data_ov100_02148188[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov100_02148188 */
int *Door_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(328);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)data_ov100_02148188;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
