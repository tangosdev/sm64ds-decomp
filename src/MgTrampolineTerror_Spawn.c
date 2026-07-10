//cpp
// NONMATCHING: register allocation (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
void* _ZN9ActorBasenwEj(unsigned int sz);
void func_ov004_020b2adc(void* p);
void _ZN8Particle10SysTrackerC1Ev(void* p);
int func_020733a8(void* p, int a, int b, void* ctor, void* dtor);
void _ZN6Player29TryExitCharacterDoorWithIntroEv(void);
void func_ov006_020ca604(void);
void func_ov006_020d1008(void);
void func_ov006_020d100c(void);
void func_ov006_021227c8(void);
void func_ov006_021248a8(void);
void func_ov006_020eed64(void);
void func_ov006_020efc08(void);
void func_ov006_02122c68(void);
void func_ov006_02122c90(void);
void func_ov006_02120938(void);
void func_ov006_021225a8(void);
extern void* _ZTV17MgBounceAndPounce;
extern void* data_ov006_0213fc7c;
}

struct Trivial { char pad[0xbc]; Trivial(){} };

extern "C" void* MgTrampolineTerror_Spawn(void){
  char *c = (char*)_ZN9ActorBasenwEj(0x7bac);
  if(c){
    func_ov004_020b2adc(c);
    *(void**)c = &_ZTV17MgBounceAndPounce;
    *(short*)(c+0x4664) = 0;
    Trivial *p = (Trivial*)(c+0x466c);
    Trivial *end = (Trivial*)(c+0x47e4);
    do { p++; } while(p != end);
    _ZN8Particle10SysTrackerC1Ev(c+0x47e4);
    *(void**)c = &data_ov006_0213fc7c;
    func_020733a8(c+0x500c, 5, 0xdc, (void*)func_ov006_020ca604, (void*)_ZN6Player29TryExitCharacterDoorWithIntroEv);
    func_020733a8(c+0x5458, 3, 0x32c, (void*)func_ov006_020d1008, (void*)func_ov006_020d100c);
    func_020733a8(c+0x5ddc, 0xa, 0x1d0, (void*)func_ov006_021227c8, (void*)func_ov006_021248a8);
    func_020733a8(c+0x6ffc, 0xa, 0x24, (void*)func_ov006_020eed64, (void*)func_ov006_020efc08);
    func_020733a8(c+0x7164, 0x14, 0x78, (void*)func_ov006_02122c68, (void*)func_ov006_02122c90);
    func_020733a8(c+0x7ad0, 5, 0x24, (void*)func_ov006_02120938, (void*)func_ov006_021225a8);
  }
  return c;
}
