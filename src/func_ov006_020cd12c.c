// @symbol func_ov006_020cd12c
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int data_ov006_0213b2c4[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213b2c4 */
int *func_ov006_020cd12c(int *t)
{
    data_ov007_020cd72c(t);
    t[0] = (int)data_ov006_0213b2c4;
    _ZN9ModelAnimC1Ev((char *)t + 0x6c);
    return t;
}
