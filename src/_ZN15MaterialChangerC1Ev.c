// @symbol _ZN15MaterialChangerC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MaterialChanger.h"


typedef struct {
    unsigned int* vtable;
    char pad[0xc];
    unsigned int unk10;
} MaterialChanger;

MaterialChanger* _ZN15MaterialChangerC1Ev(MaterialChanger* this) {
    _ZN9AnimationC2Ev(this);
    this->vtable = _ZTV15MaterialChanger;
    this->unk10 = 0;
    return this;
}
