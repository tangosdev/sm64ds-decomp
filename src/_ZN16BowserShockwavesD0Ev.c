// @symbol _ZN16BowserShockwavesD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV9daFRing_c */
extern void _ZN15MaterialChangerD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *G0;
int *_ZN16BowserShockwavesD0Ev(int *t)
{
    t[0] = (int)_ZTV9daFRing_c;
    _ZN18TextureTransformerD1Ev((char *)t + 0x200);
    _ZN15MaterialChangerD1Ev((char *)t + 0x1ec);
    _ZN15TextureSequenceD1Ev((char *)t + 0x1d8);
    _ZN9ModelAnimD1Ev((char *)t + 0x174);
    _ZN18TextureTransformerD1Ev((char *)t + 0x160);
    _ZN15MaterialChangerD1Ev((char *)t + 0x14c);
    _ZN15TextureSequenceD1Ev((char *)t + 0x138);
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
