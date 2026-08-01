// @symbol _ZN18TextureTransformerC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "TextureTransformer.h"


typedef struct {
    unsigned int* vtable;
    char pad[0xc];
    unsigned int unk10;
} TextureTransformer;

TextureTransformer* _ZN18TextureTransformerC1Ev(TextureTransformer* this) {
    _ZN9AnimationC2Ev(this);
    this->vtable = _ZTV18TextureTransformer;
    this->unk10 = 0;
    return this;
}
