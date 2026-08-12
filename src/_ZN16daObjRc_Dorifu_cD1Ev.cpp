//cpp
// @symbol _ZN16daObjRc_Dorifu_cD1Ev
/* recovered: named members + shared header */
#include "daObjRc_Dorifu_c.h"
extern "C" {
extern int __destroy_arr(void*,int,int,void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD2Ev(void*);
extern int _ZTV16daObjRc_Dorifu_c[];
extern int data_ov002_02108d94[];
extern int _ZTV8Platform[];
void* _ZN16daObjRc_Dorifu_cD1Ev(struct daObjRc_Dorifu_c *self) {
  *(int**)((char*)self)=_ZTV16daObjRc_Dorifu_c;
  *(int**)((char*)self)=data_ov002_02108d94;
  __destroy_arr(((char*)self)+0x4b0,5,0x1c8,(void*)_ZN18MovingMeshColliderD1Ev);
  __destroy_arr(((char*)self)+0x320,5,0x50,(void*)_ZN5ModelD1Ev);
  *(int**)((char*)self)=_ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev((char*)&self->mMovingMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN5ActorD2Ev(((char*)self));
  return ((char*)self);
}
}
