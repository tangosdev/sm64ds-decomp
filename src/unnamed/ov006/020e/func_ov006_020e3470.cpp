//cpp
// @symbol func_ov006_020e3470
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCurling_c.h"
// recovered name: dScMgCurling_c_OnYoshiTryEat_020e3470
/* One of dScMgBase_c's own undeclared new vtable slots (18-35, see
   include/dScMgBase_c.h) -- NOT a destructor despite the inherited
   "OnYoshiTryEat" label (same tree-wide mislabel documented in
   notes/dscene-c-siblings-census.md section 3): this body sets two
   fields and calls two helpers, nothing like a vtable-write-then-
   deallocate shape. Converted .c -> .cpp: dScMgCurling_c.h now
   transitively includes dScene_c.h, which has no C spelling. */
extern "C" {
void func_ov006_020e3388(void*);
void func_ov006_020e3250(void*);
void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void*,unsigned short,unsigned short,unsigned short,unsigned short);
int func_ov004_020adc1c(void);
void func_ov006_020e3470(unsigned char* c){
    struct dScMgCurling_c *self = (struct dScMgCurling_c *)(void *)c;
  self->unk_4eac=0;
  func_ov006_020e3388(c);
  func_ov006_020e3250(c);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4000050,0,0xd,2,0x10);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4001050,0,4,2,0x10);
  self->unk_4ed8=func_ov004_020adc1c();
}
}
