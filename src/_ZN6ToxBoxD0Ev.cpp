//cpp
extern "C" {
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern int _ZTV6ToxBox[];
extern int _ZTV17ExclamationSwitch[];
extern void* data_020a0eac;
void* _ZN6ToxBoxD0Ev(char* self){
  *(int**)(self)=_ZTV6ToxBox;
  _ZN25MovingCylinderClsnWithPosD1Ev(self+0x4e8);
  _ZN12WithMeshClsnD1Ev(self+0x324);
  *(int**)(self)=_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(self+0x124);
  _ZN5ModelD1Ev(self+0xd4);
  _ZN5ActorD1Ev(self);
  _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
  return self;
}
}
