//cpp
// @symbol daObjCtMecha04_c_classInit_CT_MECHA04L
#include "daObjCtMecha04_c.h"

/* This compiler rejects both the cartridge's unsigned-int operator-new
 * spelling and placement new. Keep that ABI boundary explicit, but let the
 * class layout name every constructed subobject. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN10dBgActor_cC2Ev(void *self);
void _ZN18TextureTransformerC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);
extern void *_ZTV16daObjCtMecha04_c;
}

/* Reconstructed source-style name: SM64DS proves daObjCtMecha04_c through RTTI,
 * allocation size, vtable identity, and the CT_MECHA04L registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: TtcConveyorBeltLarge_Spawn. */
extern "C" daObjCtMecha04_c *daObjCtMecha04_c_classInit_CT_MECHA04L()
{
    daObjCtMecha04_c *actor =
        (daObjCtMecha04_c *)_ZN7fBase_cnwEj(sizeof(daObjCtMecha04_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(void **)actor = &_ZTV16daObjCtMecha04_c;
        _ZN18TextureTransformerC1Ev(&actor->mTextureTransformer);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
    }
    return actor;
}
