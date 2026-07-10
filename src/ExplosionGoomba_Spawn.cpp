//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void func_020aed98(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void _ZN9ModelAnimC1Ev(void*);
extern void _ZN15MaterialChangerC1Ev(void*);
extern void _ZN15TextureSequenceC1Ev(void*);
extern void _ZN18TextureTransformerC1Ev(void*);
extern void _ZN12WithMeshClsnC1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN11ShadowModelC1Ev(void*);
extern void func_020072c0(void*);
extern void func_0203d384(void*);
extern void* _ZTV8Goomboss[];
void* ExplosionGoomba_Spawn(void){
  char* p=(char*)_ZN9ActorBasenwEj(0x610);
  if(p){
    func_020aed98(p);
    *(void***)p=(void**)_ZTV8Goomboss;
    func_020733a8(p+0x110, 4, 0x40, _ZN25MovingCylinderClsnWithPosC1Ev, _ZN25MovingCylinderClsnWithPosD1Ev);
    _ZN9ModelAnimC1Ev(p+0x210);
    func_020733a8(p+0x274, 3, 0x28, _ZN11ShadowModelC1Ev, _ZN11ShadowModelD1Ev);
    func_020733a8(p+0x3ac, 3, 0xc, func_0203d384, func_020072c0);
    _ZN15MaterialChangerC1Ev(p+0x3d0);
    _ZN15TextureSequenceC1Ev(p+0x3e4);
    _ZN18TextureTransformerC1Ev(p+0x3f8);
    _ZN12WithMeshClsnC1Ev(p+0x40c);
  }
  return p;
}
}
