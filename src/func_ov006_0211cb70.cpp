//cpp
extern "C" void* _ZN9ActorBasenwEj(unsigned int);
extern "C" void func_ov004_020b2adc(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_ov006_020c33dc(void*);
extern int data_ov006_0213e448;
extern int data_ov006_0213f844;
extern "C" void* func_ov006_0211cb70(void){
  char* p = (char*)_ZN9ActorBasenwEj(0x562c);
  if (p) {
    func_ov004_020b2adc(p);
    *(int*)p = (int)&data_ov006_0213e448;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&data_ov006_0213f844;
    func_ov006_020c33dc(p + 0x4f38);
  }
  return p;
}
