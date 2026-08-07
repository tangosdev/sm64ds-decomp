// @symbol _ZN8GoombossD1Ev
/* recovered: named members + shared header */
#include "Goomboss.h"
extern int __destroy_arr(void* p, int a, int b, void* d);
extern void _ZN12WithMeshClsnD1Ev(void* p);
extern void _ZN18TextureTransformerD1Ev(void* p);
extern void _ZN15TextureSequenceD1Ev(void* p);
extern void _ZN15MaterialChangerD1Ev(void* p);
extern void _ZN9ModelAnimD1Ev(void* p);
extern int _ZN5EnemyD2Ev(int* x);
extern int func_020072c0;
extern int _ZN11ShadowModelD1Ev;
extern int _ZN25MovingCylinderClsnWithPosD1Ev;
extern int _ZTV8Goomboss[];
int _ZN8GoombossD1Ev(struct Goomboss *self) {
  *(int**)((int*)self) = _ZTV8Goomboss;
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  _ZN18TextureTransformerD1Ev((char*)&self->mTextureTransformer);
  _ZN15TextureSequenceD1Ev((char*)&self->mTextureSequence);
  _ZN15MaterialChangerD1Ev((char*)&self->mMaterialChanger);
  __destroy_arr((char*)((int*)self)+0x3ac, 3, 0xc, &func_020072c0);
  __destroy_arr((char*)((int*)self)+0x274, 3, 0x28, &_ZN11ShadowModelD1Ev);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  __destroy_arr((char*)((int*)self)+0x110, 4, 0x40, &_ZN25MovingCylinderClsnWithPosD1Ev);
  _ZN5EnemyD2Ev(((int*)self));
  return (int)((int*)self);
}
