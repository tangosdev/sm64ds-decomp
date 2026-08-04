// @symbol func_ov006_020e3470
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCurling_c.h"
// recovered name: dScMgCurling_c_OnYoshiTryEat_020e3470
/* recovered: renamed to Class_Method */
/* dScMgCurling_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void func_ov006_020e3388(void*);
extern void func_ov006_020e3250(void*);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void*,unsigned short,unsigned short,unsigned short,unsigned short);
extern int func_ov004_020adc1c(void);
void func_ov006_020e3470(unsigned char* c){
    struct dScMgCurling_c *self = (struct dScMgCurling_c *)(void *)c;
  self->unk_4eac=0;
  func_ov006_020e3388(c);
  func_ov006_020e3250(c);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4000050,0,0xd,2,0x10);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4001050,0,4,2,0x10);
  self->unk_4ed8=func_ov004_020adc1c();
}
