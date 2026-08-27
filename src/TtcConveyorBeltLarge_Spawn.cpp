//cpp
// @symbol TtcConveyorBeltLarge_Spawn
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

extern "C" daObjCtMecha04_c *TtcConveyorBeltLarge_Spawn()
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
