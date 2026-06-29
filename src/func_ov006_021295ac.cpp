//cpp
extern "C" {
void func_ov004_020b2adc(void* p);
void _ZN8Particle10SysTrackerC1Ev(void* p);
void _ZN5ModelC1Ev(void* p);
void func_020733a8(void* arr, int count, int size, void* ctor, void* dtor);
}
extern void* data_ov006_0213e448;
extern void* data_ov006_0214000c;
extern "C" {
void func_0203d738(void*);
void func_0203d47c(void*);
void func_ov006_0212968c(void*);
void func_ov006_02125800(void*);
}
extern "C" void* func_ov006_021295ac(char* t){
  func_ov004_020b2adc(t);
  *(void**)t = &data_ov006_0213e448;
  _ZN8Particle10SysTrackerC1Ev(t + 0x471c);
  *(void**)t = &data_ov006_0214000c;
  _ZN5ModelC1Ev(t + 0xaba4);
  *(int*)(t + 0xabf4) = 0;
  func_020733a8(t + 0xacd8, 0x80, 8, (void*)func_0203d738, (void*)func_0203d47c);
  func_020733a8(t + 0xb5d8, 0x80, 8, (void*)func_0203d738, (void*)func_0203d47c);
  func_020733a8(t + 0xba14, 0x20, 0x24, (void*)func_ov006_0212968c, (void*)func_ov006_02125800);
  return t;
}
