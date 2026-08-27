//cpp
// @symbol TtcConveyorBeltSmall_Spawn
#include "daObjCtMecha04_c.h"

/* The small conveyor is a second spawn entry for the same RTTI class. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN10dBgActor_cC2Ev(void *self);
void _ZN18TextureTransformerC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);
extern void *_ZTV16daObjCtMecha04_c;
}

extern "C" daObjCtMecha04_c *TtcConveyorBeltSmall_Spawn()
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
