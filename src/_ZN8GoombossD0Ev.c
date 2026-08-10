// @symbol _ZN8GoombossD0Ev
/* recovered: named members + shared header */
#include "Goomboss.h"
extern int __destroy_arr(void* c, int a, int b, void* d);
extern void* _ZTV8Goomboss[];
extern void _ZN7Vector3D1Ev(void);
extern int _ZN12WithMeshClsnD1Ev(void* c);
extern int _ZN18TextureTransformerD1Ev(void* c);
extern int _ZN15TextureSequenceD1Ev(void* c);
extern int _ZN15MaterialChangerD1Ev(void* c);
extern int _ZN11ShadowModelD1Ev(void* c);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void* c);
extern int _ZN9ModelAnimD1Ev(void* c);
extern int _ZN5EnemyD2Ev(int* x);
extern int data_020a0eac;
extern int _ZN6Memory10DeallocateEPvP4Heap(void* p, void* h);
int _ZN8GoombossD0Ev(struct Goomboss *self) {
    *(void**)((void*)self) = _ZTV8Goomboss;
    _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
    _ZN18TextureTransformerD1Ev((char*)&self->mTextureTransformer);
    _ZN15TextureSequenceD1Ev((char*)&self->mTextureSequence);
    _ZN15MaterialChangerD1Ev((char*)&self->mMaterialChanger);
    __destroy_arr((char*)((void*)self)+0x3ac, 3, 0xc, (void*)_ZN7Vector3D1Ev);
    __destroy_arr((char*)((void*)self)+0x274, 3, 0x28, (void*)_ZN11ShadowModelD1Ev);
    _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
    __destroy_arr((char*)((void*)self)+0x110, 4, 0x40, (void*)_ZN25MovingCylinderClsnWithPosD1Ev);
    _ZN5EnemyD2Ev((int*)((void*)self));
    _ZN6Memory10DeallocateEPvP4Heap(((void*)self), (void*)data_020a0eac);
    return (int)((void*)self);
}
