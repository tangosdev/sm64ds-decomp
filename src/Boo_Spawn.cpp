//cpp
extern "C" {
typedef unsigned int u32;
void* _ZN9ActorBasenwEj(u32 sz);
void _ZN8CapEnemyC2Ev(void* t);
void _ZN25MovingCylinderClsnWithPosC1Ev(void* t);
void _ZN12WithMeshClsnC1Ev(void* t);
void _ZN9ModelAnimC1Ev(void* t);
void _ZN5ModelC1Ev(void* t);
void _ZN11ShadowModelC1Ev(void* t);
extern void* _ZTV3Boo;
void* Boo_Spawn(void){
  char* t=(char*)_ZN9ActorBasenwEj(0x5e0);
  if(t){
    _ZN8CapEnemyC2Ev(t);
    *(void**)t = &_ZTV3Boo;
    _ZN25MovingCylinderClsnWithPosC1Ev(t+0x184);
    _ZN12WithMeshClsnC1Ev(t+0x1c4);
    _ZN9ModelAnimC1Ev(t+0x380);
    _ZN5ModelC1Ev(t+0x3e4);
    _ZN11ShadowModelC1Ev(t+0x434);
    _ZN11ShadowModelC1Ev(t+0x45c);
  }
  return t;
}
}
