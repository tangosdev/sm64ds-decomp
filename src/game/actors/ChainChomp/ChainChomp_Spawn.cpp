//cpp
extern "C" {
void* _ZN7fBase_cnwEj(unsigned int);
void _ZN12dEnemyBase_cC2Ev(void*);
int _ZN10dCcAcPos_cC1Ev(void*);
int _ZN9ModelAnimC1Ev(void*);
int _ZN11ShadowModelC1Ev(void*);
int func_020733a8(void*, int, int, void*, void*);
extern int _ZTV10ChainChomp[];
extern void _ZN5ModelD1Ev();
extern void _ZN5ModelC1Ev();
extern void _ZN11ShadowModelD1Ev();
extern void _ZN7Vector3D1Ev();
extern void func_0203d384();
void* ChainChomp_Spawn(void){
  char* c = (char*)_ZN7fBase_cnwEj(0x620);
  if(c){
    _ZN12dEnemyBase_cC2Ev(c);
    *(int**)c = _ZTV10ChainChomp;
    _ZN10dCcAcPos_cC1Ev(c+0x110);
    _ZN9ModelAnimC1Ev(c+0x150);
    _ZN11ShadowModelC1Ev(c+0x1b4);
    func_020733a8(c+0x1dc, 7, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    func_020733a8(c+0x40c, 7, 0x28, (void*)_ZN11ShadowModelC1Ev, (void*)_ZN11ShadowModelD1Ev);
    func_020733a8(c+0x524, 7, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
    func_020733a8(c+0x578, 7, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
  }
  return c;
}
}
