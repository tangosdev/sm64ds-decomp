//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int sz);
extern void _ZN9ActorBaseC1Ev(void* p);
extern void _ZN8Particle10SysTrackerC1Ev(void* p);
extern void _ZN5ModelC1Ev(void* p);
extern void _ZN9ModelAnimC1Ev(void* p);
extern void func_020733a8(void* a, int b, int c, void* d, void* e);
extern void* _ZTV12ActorDerived;
extern void* data_ov075_0211c6a0;
extern void func_ov075_02113fdc(void);
extern void func_ov075_02115a88(void);
}
extern "C" char* func_ov075_021159f4(void);
extern "C" char* func_ov075_021159f4(void){
  char* p = (char*)_ZN9ActorBasenwEj(0xf48);
  if (p) {
    _ZN9ActorBaseC1Ev(p);
    *(void**)p = &_ZTV12ActorDerived;
    *(void**)p = &data_ov075_0211c6a0;
    _ZN8Particle10SysTrackerC1Ev(p + 0x50);
    _ZN5ModelC1Ev(p + 0x86c);
    _ZN9ModelAnimC1Ev(p + 0x8bc);
    func_020733a8(p + 0x920, 4, 0x158, (void*)func_ov075_02115a88, (void*)func_ov075_02113fdc);
  }
  return p;
}
