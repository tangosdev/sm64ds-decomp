//cpp
extern "C" void* _ZN7fBase_cnwEj(unsigned int);
extern "C" void _ZN11dScMgBase_cC2Ev(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_ov006_020c1d80(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV13dScMgMCarlo_c;
/* The two callbacks handed to func_020733a8 are the element ctor and dtor
 * for the 0x50 x 0x30 array at 0x51a8; the dtor takes the element. Spelled
 * as it is recovered in the TU (src/actors/dScMgMCarlo_c.cpp) -- the call
 * below casts to void* either way, so this costs no code and stops the two
 * files disagreeing about the one fact that sizes the array. */
extern "C" void func_ov006_020f7730(int *p);
extern "C" void func_ov006_020f8ed8(void);
extern "C" void* func_ov006_020f8e44(void){
  char* p = (char*)_ZN7fBase_cnwEj(0x60b0);
  if (p) {
    _ZN11dScMgBase_cC2Ev(p);
    *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&_ZTV13dScMgMCarlo_c;
    func_ov006_020c1d80(p + 0x4f38);
    func_020733a8(p + 0x51a8, 0x50, 0x30, (void*)func_ov006_020f8ed8, (void*)func_ov006_020f7730);
  }
  return p;
}
