//cpp
extern "C" {
void* _ZN9ActorBasenwEj(unsigned int);
void _ZN8PlatformC2Ev(void*);
void _ZN9ModelAnimC1Ev(void*);
void _ZN11ShadowModelC1Ev(void*);
void func_020733a8(void*, int, int, void*, void*);
void _ZN25MovingCylinderClsnWithPosC1Ev(void*);
void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
void _ZN12WithMeshClsnC1Ev(void*);
extern int _ZTV8MadPiano[];
void* MadPiano_Spawn(void) {
  char* r4 = (char*)_ZN9ActorBasenwEj(0x6e4);
  if (r4) {
    _ZN8PlatformC2Ev(r4);
    *(int*)r4 = (int)_ZTV8MadPiano;
    _ZN9ModelAnimC1Ev(r4 + 0x320);
    _ZN11ShadowModelC1Ev(r4 + 0x384);
    _ZN11ShadowModelC1Ev(r4 + 0x3ac);
    _ZN11ShadowModelC1Ev(r4 + 0x3d4);
    func_020733a8(r4 + 0x48c, 2, 0x40, (void*)_ZN25MovingCylinderClsnWithPosC1Ev, (void*)_ZN25MovingCylinderClsnWithPosD1Ev);
    _ZN12WithMeshClsnC1Ev(r4 + 0x50c);
  }
  return r4;
}
}
