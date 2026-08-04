//cpp
extern "C" void* _ZN9ActorBasenwEj(unsigned int);
extern "C" void func_ov004_020b2adc(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern "C" void func_ov006_020c3f54(void*);
extern int data_ov006_0213e448;
extern int data_ov006_02140140;
extern "C" void func_ov006_0212a650(void);
extern "C" void func_ov006_0212b88c(void);
extern "C" void* func_ov006_0212b7f8(void){
  char* p = (char*)_ZN9ActorBasenwEj(0x5ff8);
  if (p) {
    func_ov004_020b2adc(p);
    *(int*)p = (int)&data_ov006_0213e448;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&data_ov006_02140140;
    func_020733a8(p + 0x4f38, 0x16, 0x20, (void*)func_ov006_0212b88c, (void*)func_ov006_0212a650);
    func_ov006_020c3f54(p + 0x51f8);
  }
  return p;
}
