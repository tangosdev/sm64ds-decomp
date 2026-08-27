// @symbol func_ov006_020e6bf4
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213c510[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213c510 */
int *func_ov006_020e6bf4(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(21956);
    if (p) {
        func_ov004_020b2adc(p);
        p[0] = (int)data_ov006_0213c510;
    }
    return p;
}
