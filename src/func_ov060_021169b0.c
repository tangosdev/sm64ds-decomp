// @symbol func_ov060_021169b0
// @emits Bowser_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_WithMeshClsn.h"
/* recovered: renamed to Class_Method */
/* daKpa_c::Kill - recovered from vtable slot identity */
extern int RandomIntInternal(int* seed);
extern int data_0209e650;

void Bowser_Kill(char* thiz) {
    *(int*)(thiz + 0xa8) = 0x1e000;
    *(int*)(thiz + 0x98) = 0xf000;
    *(short*)(thiz + 0x376) = (unsigned int)RandomIntInternal(&data_0209e650) >> 16;
    _ZN12WithMeshClsn13SetLimMovFlagEv(thiz + 0x110);
    *(int*)(thiz + 0x368) = 0x10;
}
