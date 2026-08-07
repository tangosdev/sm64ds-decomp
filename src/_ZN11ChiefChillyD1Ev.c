// @symbol _ZN11ChiefChillyD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_BlendModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
/* recovered: named members + shared header */
#include "ChiefChilly.h"
extern int __destroy_arr(void* p, int a, int b, void* d);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void* p);
extern int _ZN5EnemyD2Ev(int* x);
extern int func_020072c0;
extern int _ZTV11ChiefChilly[];
int _ZN11ChiefChillyD1Ev(struct ChiefChilly *self) {
  *(int**)((int*)self) = _ZTV11ChiefChilly;
  __destroy_arr((char*)((int*)self)+0x4d4, 2, 0xc, &func_020072c0);
  __destroy_arr((char*)((int*)self)+0x448, 8, 0xc, &func_020072c0);
  __destroy_arr((char*)((int*)self)+0x3e8, 8, 0xc, &func_020072c0);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
  _ZN14BlendModelAnimD1Ev((char*)&self->mBlendModelAnim);
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
  _ZN5EnemyD2Ev(((int*)self));
  return (int)((int*)self);
}
