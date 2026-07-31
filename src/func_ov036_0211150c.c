// @symbol func_ov036_0211150c
// @emits daObjRc_Kaitendai_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjRc_Kaitendai_c::InitResources - recovered from vtable slot identity */
extern int data_ov036_02113b2c;
extern int func_ov002_020b676c(int *self, void *arg, int val);

int daObjRc_Kaitendai_c_InitResources(int *self)
{
    short v;
    v = data_ov036_02113b18;
    if ((self[2] & 0xff) == 1) {
        v = data_ov036_02113b1c;
    }
    return func_ov002_020b676c(self, &data_ov036_02113b2c, v);
}
