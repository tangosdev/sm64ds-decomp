//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int sz);
extern void _ZN9ActorBaseC1Ev(void* p);
extern void _ZN8Particle10SysTrackerC1Ev(void* p);
extern void _ZN5ModelC1Ev(void* p);
extern void _ZN9ModelAnimC1Ev(void* p);
extern void func_020733a8(void* a, int b, int c, void* d, void* e);
extern void* data_0208e4b8;
extern void* _ZTV14UnknownVsEntry;
extern void func_ov075_02113fdc(void);
extern void func_ov075_02115a88(void);
}
extern "C" char* UnknownVsEntry_Spawn(void);
extern "C" char* UnknownVsEntry_Spawn(void){
  char* p = (char*)_ZN9ActorBasenwEj(0xf48);
  if (p) {
    _ZN9ActorBaseC1Ev(p);
    *(void**)p = &data_0208e4b8;
    *(void**)p = &_ZTV14UnknownVsEntry;
    _ZN8Particle10SysTrackerC1Ev(p + 0x50);
    _ZN5ModelC1Ev(p + 0x86c);
    _ZN9ModelAnimC1Ev(p + 0x8bc);
    func_020733a8(p + 0x920, 4, 0x158, (void*)func_ov075_02115a88, (void*)func_ov075_02113fdc);
  }
  return p;
}
