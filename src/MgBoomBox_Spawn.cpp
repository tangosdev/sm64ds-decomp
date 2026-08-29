//cpp
extern "C" void* _ZN7fBase_cnwEj(unsigned int);
extern "C" void _ZN11dScMgBase_cC2Ev(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_ov006_020c33dc(void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV12dScMgSound_c;
extern "C" void* MgBoomBox_Spawn(void){
  char* p = (char*)_ZN7fBase_cnwEj(0x562c);
  if (p) {
    _ZN11dScMgBase_cC2Ev(p);
    *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&_ZTV12dScMgSound_c;
    func_ov006_020c33dc(p + 0x4f38);
  }
  return p;
}
