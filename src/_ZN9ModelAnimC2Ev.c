// @symbol _ZN9ModelAnimC2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ModelAnim.h"


typedef struct {
    unsigned int* vtable;
    char pad_model[0x4c];
    unsigned int* anim_vtable;
    char pad_anim[0xc];
    unsigned int unk60;
} ModelAnim;

ModelAnim* _ZN9ModelAnimC2Ev(struct ModelAnim *self) {
    _ZN5ModelC2Ev(((ModelAnim*)self));
    _ZN9AnimationC2Ev((char*)&self->mAnimation);
    ((ModelAnim*)self)->vtable = _ZTV9ModelAnim;
    ((ModelAnim*)self)->anim_vtable = VTable_Animation_ModelAnimThunk;
    ((ModelAnim*)self)->unk60 = 0;
    return ((ModelAnim*)self);
}
