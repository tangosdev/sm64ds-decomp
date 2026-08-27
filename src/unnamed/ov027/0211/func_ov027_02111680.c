// @symbol func_ov027_02111680
// recovered name: daIDonketu_c_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daIDonketu_c::Kill - recovered from vtable slot identity */
int func_ov027_02111680(char* self){
    if(*(unsigned short*)(self+0x100) < 0xa){
        *(int*)(self+0x98) = 0;
        int r = func_ov064_02116110(self, 0x700);
        if(r != 0) return r;
        unsigned short* p = (unsigned short*)(self+0x100);
        if(*p == 9) *p = 0;
        return (int)p;
    }
    *(int*)(self+0x98) = 0x14000;
    unsigned short* p = (unsigned short*)(self+0x100);
    unsigned int v = *p;
    if(v > 0x23){ v = 0; *p = 0; }
    return v;
}
