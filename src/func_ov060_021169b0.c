// @symbol func_ov060_021169b0
// recovered name: Bowser_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
/* recovered: renamed to Class_Method */
/* daKpa_c::Kill - recovered from vtable slot identity */
extern int RandomIntInternal(int* seed);
extern int data_0209e650;

void func_ov060_021169b0(char* thiz) {
    *(int*)(thiz + 0xa8) = 0x1e000;
    *(int*)(thiz + 0x98) = 0xf000;
    *(short*)(thiz + 0x376) = (unsigned int)RandomIntInternal(&data_0209e650) >> 16;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(thiz + 0x110);
    *(int*)(thiz + 0x368) = 0x10;
}
