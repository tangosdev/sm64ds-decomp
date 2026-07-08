//cpp
extern "C" {
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
extern void* data_ov092_021322d0;
extern void* _ZTV8Platform;
void* func_ov092_02130f00(char* c){
  *(void**)c = &data_ov092_021322d0;
  _ZN25MovingCylinderClsnWithPosD1Ev(c+0x4e8);
  _ZN12WithMeshClsnD1Ev(c+0x324);
  *(void**)c = &_ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
}
