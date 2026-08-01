// @symbol _ZN11ChiefChillyD0Ev
/* recovered: named members + shared header */
#include "ChiefChilly.h"
extern int __destroy_arr(void* c, int a, int b, void* d);
extern void* _ZTV11ChiefChilly[];
extern void func_020072c0(void);
extern int _ZN11ShadowModelD1Ev(void* c);
extern int _ZN14BlendModelAnimD1Ev(void* c);
extern int _ZN12WithMeshClsnD1Ev(void* c);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void* c);
extern int func_ov002_020aed18(int* x);
extern int data_020a0eac;
extern int _ZN6Memory10DeallocateEPvP4Heap(void* p, void* h);
int _ZN11ChiefChillyD0Ev(struct ChiefChilly *self) {
    *(void**)((void*)self) = _ZTV11ChiefChilly;
    __destroy_arr((char*)((void*)self)+0x4d4, 2, 0xc, (void*)func_020072c0);
    __destroy_arr((char*)((void*)self)+0x448, 8, 0xc, (void*)func_020072c0);
    __destroy_arr((char*)((void*)self)+0x3e8, 8, 0xc, (void*)func_020072c0);
    _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
    _ZN14BlendModelAnimD1Ev((char*)&self->mBlendModelAnim);
    _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
    func_ov002_020aed18((int*)((void*)self));
    _ZN6Memory10DeallocateEPvP4Heap(((void*)self), (void*)data_020a0eac);
    return (int)((void*)self);
}
