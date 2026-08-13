//cpp
// @symbol func_ov006_020edf54
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void func_ov006_020c6f3c();
extern void *_ZTV14dScMgD3DBase_c[];
extern void *data_020a0eac;
void *func_ov006_020edf54(char *c);
void *func_ov006_020edf54(char *c) {
    *(void ***)c = data_ov006_0213cbe4;
    __destroy_arr(c + 0x5294, 6, 0xf0, (void*)&func_ov006_020c6f3c);
    __destroy_arr(c + 0x506c, 3, 0xb8, (void*)&func_ov006_020c893c);
    _ZN5ModelD1Ev(c + 0x501c);
    *(void ***)c = _ZTV14dScMgD3DBase_c;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    _ZN11dScMgBase_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
