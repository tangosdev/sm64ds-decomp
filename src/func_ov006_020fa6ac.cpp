//cpp
extern "C" void* _ZN9ActorBasenwEj(unsigned int);
extern "C" void func_ov004_020b2adc(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_ov006_020c1d80(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern int data_ov006_0213e448;
extern int data_ov006_0213d7e8;
extern "C" void func_ov006_020fa740(void);
extern "C" void func_ov006_020f8ff0(void);
extern "C" void* func_ov006_020fa6ac(void){
  char* p = (char*)_ZN9ActorBasenwEj(0x5930);
  if (p) {
    func_ov004_020b2adc(p);
    *(int*)p = (int)&data_ov006_0213e448;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&data_ov006_0213d7e8;
    func_ov006_020c1d80(p + 0x4f38);
    func_020733a8(p + 0x51a8, 0x28, 0x30, (void*)func_ov006_020fa740, (void*)func_ov006_020f8ff0);
  }
  return p;
}
