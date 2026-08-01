//cpp
// @symbol func_ov006_020ed18c
// recovered name: dScMgHanachan_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgHanachan_c::Behavior - recovered from vtable slot identity */


struct C;
typedef void (C::*PMF)();

extern "C" int func_ov006_020ed18c(char *c)
{
    data_ov006_02141fcc = data_ov006_02141fcc + 0x800;
    (((C *)c)->*(*(PMF *)(c + 0x4660)))();
    int i = 0;
    if (data_ov006_0213c958 > 0) {
        char *p = c + 0x4678;
        do {
            func_ov006_020eb610(p);
            i++;
            p += 0x98;
        } while (i < data_ov006_0213c958);
    }
    int j = 0;
    if (data_ov006_0213c958 > 0) {
        char *q = c + 0x4678;
        do {
            func_ov006_020eb558(q);
            j++;
            q += 0x98;
        } while (j < data_ov006_0213c958);
    }
    func_ov006_020ea71c();
    func_ov006_020ea3d0(c + 0x4670);
    return 1;
}
