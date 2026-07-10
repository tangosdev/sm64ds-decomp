//cpp
extern "C" void* _ZN9ActorBasenwEj(unsigned int);
extern "C" void func_ov004_020b2adc(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_ov006_020c1d80(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern int data_ov006_0213e448;
extern int data_ov006_0213bdb4;
extern "C" void func_ov006_020d96e0(void);
extern "C" void func_ov006_020dbe30(void);
extern "C" void func_ov006_020d96f0(void);
extern "C" void func_ov006_020dbe14(void);
extern "C" void* MgPicturePoker_Spawn(void){
  char* p = (char*)_ZN9ActorBasenwEj(0x539c);
  if (p) {
    func_ov004_020b2adc(p);
    *(int*)p = (int)&data_ov006_0213e448;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&data_ov006_0213bdb4;
    func_ov006_020c1d80(p + 0x4f38);
    func_020733a8(p + 0x51a8, 5, 0x30, (void*)func_ov006_020dbe30, (void*)func_ov006_020d96e0);
    func_020733a8(p + 0x5298, 5, 0x30, (void*)func_ov006_020dbe14, (void*)func_ov006_020d96f0);
  }
  return p;
}
