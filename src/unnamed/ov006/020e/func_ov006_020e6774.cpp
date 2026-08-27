//cpp
// @symbol func_ov006_020e6774
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCurling2_c.h"
// recovered name: dScMgCurling2_c_OnYoshiTryEat_020e6774
/* Same shape as func_ov006_020e3470.cpp -- see that file's note. */
extern "C" {
void func_ov006_020e668c(void*);
void func_ov006_020e6528(void*);
void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void*,unsigned short,unsigned short,unsigned short,unsigned short);
int func_ov004_020adc1c(void);
void func_ov006_020e6774(unsigned char* c){
    struct dScMgCurling2_c *self = (struct dScMgCurling2_c *)(void *)c;
  self->unk_5580=0;
  func_ov006_020e668c(c);
  func_ov006_020e6528(c);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4000050,0,0xd,2,0x10);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4001050,0,4,2,0x10);
  self->unk_55ac=func_ov004_020adc1c();
}
}
