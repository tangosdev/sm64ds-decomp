//cpp
extern "C" {
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern int data_ov092_021322d0[];
extern int data_ov002_0210ae38[];
extern void* data_020a0eac;
void* func_ov092_02130f5c(char* self){
  *(int**)(self)=data_ov092_021322d0;
  _ZN25MovingCylinderClsnWithPosD1Ev(self+0x4e8);
  _ZN12WithMeshClsnD1Ev(self+0x324);
  *(int**)(self)=data_ov002_0210ae38;
  _ZN18MovingMeshColliderD1Ev(self+0x124);
  _ZN5ModelD1Ev(self+0xd4);
  _ZN5ActorD1Ev(self);
  _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
  return self;
}
}
