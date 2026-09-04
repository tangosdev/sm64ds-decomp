// @symbol daFRing_c_classInit
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
/* Reconstructed source-style name: SM64DS proves daFRing_c through RTTI,
 * allocation size, vtable identity, and the FIRERING registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BowserShockwaves_Spawn. */
int *daFRing_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(536);
    if (p) {
        _ZN8dActor_cC2Ev(p);
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
