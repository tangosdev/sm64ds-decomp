// @symbol Seaweed_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int data_ov002_02109bb8[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02109bb8 */
int *Seaweed_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(312);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)data_ov002_02109bb8;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
