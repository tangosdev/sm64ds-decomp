//cpp
extern "C" void* _ZN9ActorBasenwEj(unsigned int sz);
extern "C" void func_ov004_020b2adc(void);
extern "C" void _ZN8Particle10SysTrackerC1Ev(char* p);
extern "C" void func_ov006_020c1d80(char* p);
extern "C" void func_020733a8(void* p, int a, int b, void* d, void* e);
extern "C" void _ZN5ModelC1Ev(char* p);
extern int data_ov006_0213e448[];
extern int data_ov006_0213e39c[];
extern int func_ov006_021079c8[];
extern int func_ov006_0210a4ac[];

extern "C" void* MgMushroomRoulette_Spawn(void){
  char* c = (char*)_ZN9ActorBasenwEj(0x5400);
  if (c) {
    func_ov004_020b2adc();
    *(int*)c = (int)data_ov006_0213e448;
    _ZN8Particle10SysTrackerC1Ev(c + 0x471c);
    *(int*)c = (int)data_ov006_0213e39c;
    func_ov006_020c1d80(c + 0x4f38);
    func_020733a8(c + 0x51a8, 5, 0x34, func_ov006_0210a4ac, func_ov006_021079c8);
    char* m = c + 0x530c;
    _ZN5ModelC1Ev(m + 0x10);
    _ZN5ModelC1Ev(m + 0x60);
  }
  return c;
}
