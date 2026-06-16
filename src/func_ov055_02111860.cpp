//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int sz);
extern void _ZN5ActorC2Ev(void* c);
extern void _ZN9ModelAnimC1Ev(void* c);
extern void _ZN5ModelC1Ev(void* c);
extern void _ZN11ShadowModelC1Ev(void* c);
extern void* func_020733a8(void* a, int b, int n, void* ctor, void* dtor);
extern int data_ov055_02111ae0[];
extern void _ZN15TextureSequenceD1Ev(void*);
extern void _ZN15TextureSequenceC1Ev(void*);
void* func_ov055_02111860(void){
  char* c = (char*)_ZN9ActorBasenwEj(0x20c);
  if (c) {
    _ZN5ActorC2Ev(c);
    *(int**)c = data_ov055_02111ae0;
    _ZN9ModelAnimC1Ev(c+0xd4);
    _ZN5ModelC1Ev(c+0x138);
    _ZN11ShadowModelC1Ev(c+0x188);
    func_020733a8(c+0x1b0, 2, 0x14, (void*)_ZN15TextureSequenceC1Ev, (void*)_ZN15TextureSequenceD1Ev);
  }
  return c;
}
}
