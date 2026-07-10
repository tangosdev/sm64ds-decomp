//cpp
extern "C" {
void* _ZN9ActorBasenwEj(unsigned int);
void func_020aed98(void);
int _ZN25MovingCylinderClsnWithPosC1Ev(void*);
int _ZN9ModelAnimC1Ev(void*);
int _ZN11ShadowModelC1Ev(void*);
int func_020733a8(void*, int, int, void*, void*);
extern int _ZTV10ChainChomp[];
extern void _ZN5ModelD1Ev();
extern void _ZN5ModelC1Ev();
extern void _ZN11ShadowModelD1Ev();
extern void func_020072c0();
extern void func_0203d384();
void* ChainChomp_Spawn(void){
  char* c = (char*)_ZN9ActorBasenwEj(0x620);
  if(c){
    func_020aed98();
    *(int**)c = _ZTV10ChainChomp;
    _ZN25MovingCylinderClsnWithPosC1Ev(c+0x110);
    _ZN9ModelAnimC1Ev(c+0x150);
    _ZN11ShadowModelC1Ev(c+0x1b4);
    func_020733a8(c+0x1dc, 7, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    func_020733a8(c+0x40c, 7, 0x28, (void*)_ZN11ShadowModelC1Ev, (void*)_ZN11ShadowModelD1Ev);
    func_020733a8(c+0x524, 7, 0xc, (void*)func_0203d384, (void*)func_020072c0);
    func_020733a8(c+0x578, 7, 0xc, (void*)func_0203d384, (void*)func_020072c0);
  }
  return c;
}
}
