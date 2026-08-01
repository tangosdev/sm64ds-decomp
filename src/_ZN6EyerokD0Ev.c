// @symbol _ZN6EyerokD0Ev
/* recovered: named members + shared header */
#include "Eyerok.h"
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void* __destroy_arr(void*, int, int, void*);
extern void _ZN15TextureSequenceD1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN14BlendModelAnimD1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN5ActorD2Ev(void*);
extern void* _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern int _ZTV6Eyerok[];
extern int func_020072c0[];
extern int _ZTV17ExclamationSwitch[];
extern void* data_020a0eac;
void* _ZN6EyerokD0Ev(struct Eyerok *self) {
    *(int**)((char*)self) = _ZTV6Eyerok;
    _ZN18MovingMeshColliderD1Ev((char*)&self->unk_674);
    __destroy_arr(((char*)self)+0x4dc, 0x14, 0xc, func_020072c0);
    _ZN15TextureSequenceD1Ev((char*)&self->mTextureSequence);
    _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
    _ZN5ModelD1Ev((char*)&self->mModel2);
    _ZN14BlendModelAnimD1Ev((char*)&self->mBlendModelAnim);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
    *(int**)((char*)self) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char*)&self->unk_124);
    _ZN5ModelD1Ev((char*)&self->mModel1);
    _ZN5ActorD2Ev(((char*)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char*)self), data_020a0eac);
    return ((char*)self);
}
