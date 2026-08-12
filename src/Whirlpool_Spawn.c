// @symbol Whirlpool_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9Whirlpool */
int *Whirlpool_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(444);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV9Whirlpool;
        _ZN9ModelAnimC1Ev((char *)p + 0x114);
        _ZN18TextureTransformerC1Ev((char *)p + 0x178);
    }
    return p;
}
