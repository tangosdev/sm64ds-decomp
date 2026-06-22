//cpp
typedef unsigned short u16;
extern "C" {
void func_ov006_02115b0c(void);
void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void* p, u16 a, u16 b, u16 c, u16 d);
void func_ov006_02118a8c(void){
  func_ov006_02115b0c();
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4000050, 0, 0x18, 4, 0xa);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4001050, 0, 0x18, 4, 0xa);
}
}
