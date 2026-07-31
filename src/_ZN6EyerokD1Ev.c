// @symbol _ZN6EyerokD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_BlendModelAnim.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Eyerok.h"
extern void _ZN15TextureSequenceD1Ev(void* p);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void* p);
extern int func_0207328c(void* p, int a, int b, void* fn);
extern int _ZTV17ExclamationSwitch[];
extern int func_020072c0[];
void* _ZN6EyerokD1Ev(struct Eyerok *self) {
    *(void**)((char*)self) = _ZTV6Eyerok;
    _ZN18MovingMeshColliderD1Ev((char*)&self->unk_674);
    func_0207328c(((char*)self)+0x4dc, 0x14, 0xc, func_020072c0);
    _ZN15TextureSequenceD1Ev((char*)&self->mTextureSequence);
    _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
    _ZN5ModelD1Ev((char*)&self->mModel2);
    _ZN14BlendModelAnimD1Ev((char*)&self->mBlendModelAnim);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
    *(void**)((char*)self) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char*)&self->unk_124);
    _ZN5ModelD1Ev((char*)&self->mModel1);
    _ZN5ActorD2Ev(((char*)self));
    return ((char*)self);
}
