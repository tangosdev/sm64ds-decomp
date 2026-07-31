// @symbol _ZN16BowserShockwavesD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV16BowserShockwaves */
extern void _ZN15MaterialChangerD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
int *_ZN16BowserShockwavesD1Ev(int *t)
{
    t[0] = (int)_ZTV16BowserShockwaves;
    _ZN18TextureTransformerD1Ev((char *)t + 0x200);
    _ZN15MaterialChangerD1Ev((char *)t + 0x1ec);
    _ZN15TextureSequenceD1Ev((char *)t + 0x1d8);
    _ZN9ModelAnimD1Ev((char *)t + 0x174);
    _ZN18TextureTransformerD1Ev((char *)t + 0x160);
    _ZN15MaterialChangerD1Ev((char *)t + 0x14c);
    _ZN15TextureSequenceD1Ev((char *)t + 0x138);
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
