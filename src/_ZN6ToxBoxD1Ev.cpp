//cpp
extern "C" {
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
extern void* _ZTV6ToxBox;
extern void* _ZTV17ExclamationSwitch;
void* _ZN6ToxBoxD1Ev(char* c){
  *(void**)c = &_ZTV6ToxBox;
  _ZN25MovingCylinderClsnWithPosD1Ev(c+0x4e8);
  _ZN12WithMeshClsnD1Ev(c+0x324);
  *(void**)c = &_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
}
