//cpp
extern "C" void* _ZN7fBase_cnwEj(unsigned int);
extern "C" void _ZN11dScMgBase_cC2Ev(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_ov006_020c1d80(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV11dScMgCard_c;
extern "C" void func_ov006_020d96e0(void);
extern "C" void func_ov006_020dbe30(void);
extern "C" void func_ov006_020d96f0(void);
extern "C" void func_ov006_020dbe14(void);
extern "C" void* MgPicturePoker_Spawn(void){
  char* p = (char*)_ZN7fBase_cnwEj(0x539c);
  if (p) {
    _ZN11dScMgBase_cC2Ev(p);
    *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&_ZTV11dScMgCard_c;
    func_ov006_020c1d80(p + 0x4f38);
    func_020733a8(p + 0x51a8, 5, 0x30, (void*)func_ov006_020dbe30, (void*)func_ov006_020d96e0);
    func_020733a8(p + 0x5298, 5, 0x30, (void*)func_ov006_020dbe14, (void*)func_ov006_020d96f0);
  }
  return p;
}
