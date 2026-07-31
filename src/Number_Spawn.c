// @symbol Number_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV15InvisibleSecret */
int *Number_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(336);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV15InvisibleSecret;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x124);
    }
    return p;
}
