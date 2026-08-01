//cpp
// @symbol _ZN6PlayerD2Ev
/* recovered: named members + shared header */
#include "Player.h"
extern "C" {
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int __destroy_arr(void*, int, int, void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN5ActorD2Ev(void*);
extern void _ZN15TextureSequenceD1Ev(void);
extern void _ZN15MaterialChangerD1Ev(void);
extern void* data_ov002_0210a83c[];
void* _ZN6PlayerD2Ev(struct Player *self) {
  *(void***)((void*)self) = data_ov002_0210a83c;
  _ZN12WithMeshClsnD1Ev((char*)&self->mMeshClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mAttackClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
  __destroy_arr((char*)((void*)self)+0x254, 2, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  __destroy_arr((char*)((void*)self)+0x22c, 2, 0x14, (void*)_ZN15MaterialChangerD1Ev);
  __destroy_arr((char*)((void*)self)+0x1dc, 4, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim4);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim3);
  _ZN5ActorD2Ev(((void*)self));
  return ((void*)self);
}
}
