//cpp
extern "C" void* _ZN7fBase_cnwEj(unsigned int);
extern "C" void _ZN11dScMgBase_cC2Ev(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern "C" void func_ov006_020c3f54(void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV13dScMgFlower_c;
extern "C" void func_ov006_0212a650(void);
extern "C" void func_ov006_0212b88c(void);
extern "C" void* func_ov006_0212b7f8(void){
  char* p = (char*)_ZN7fBase_cnwEj(0x5ff8);
  if (p) {
    _ZN11dScMgBase_cC2Ev(p);
    *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&_ZTV13dScMgFlower_c;
    func_020733a8(p + 0x4f38, 0x16, 0x20, (void*)func_ov006_0212b88c, (void*)func_ov006_0212a650);
    func_ov006_020c3f54(p + 0x51f8);
  }
  return p;
}
