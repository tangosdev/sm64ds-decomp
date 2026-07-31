// @symbol _ZN11MirrorLuigiD0Ev
/* recovered: named members + shared header */
#include "MirrorLuigi.h"
extern int _ZTV11MirrorLuigi[];
extern int _ZN15TextureSequenceD1Ev(void*);
extern int data_020a0eac[];
extern int func_0207328c(void*,int,int,void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN5ActorD2Ev(void*);
extern int _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
int _ZN11MirrorLuigiD0Ev(struct MirrorLuigi *self) {
  *(int*)((char*)self)=(int)_ZTV11MirrorLuigi;
  func_0207328c(((char*)self)+0x1b0,2,0x14,(void*)_ZN15TextureSequenceD1Ev);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN5ActorD2Ev(((char*)self));
  _ZN6Memory10DeallocateEPvP4Heap(((char*)self),(void*)data_020a0eac[0]);
  return (int)((char*)self);
}
