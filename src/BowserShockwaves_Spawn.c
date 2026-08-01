// @symbol BowserShockwaves_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_MaterialChanger.h"
#include "decl_ModelAnim.h"
#include "decl_TextureSequence.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV16BowserShockwaves */
int *BowserShockwaves_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(536);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV16BowserShockwaves;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN15MaterialChangerC1Ev((char *)p + 0x14c);
        _ZN18TextureTransformerC1Ev((char *)p + 0x160);
        _ZN9ModelAnimC1Ev((char *)p + 0x174);
        _ZN15TextureSequenceC1Ev((char *)p + 0x1d8);
        _ZN15MaterialChangerC1Ev((char *)p + 0x1ec);
        _ZN18TextureTransformerC1Ev((char *)p + 0x200);
    }
    return p;
}
