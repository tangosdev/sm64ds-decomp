//cpp
// @symbol _ZN15dScMgHanachan_c8BehaviorEv
/* dScMgHanachan_c::Behavior -- 0x4660 stores a value read/called as a
   pointer-to-member-function (see the class header); the local incomplete
   type C is only there to get the PMF calling convention, same idiom the
   pre-migration source already used. 0x4678 is this class's own 15x0x98
   array (see the class header). */
#include "decl_common.h"
#include "dScMgHanachan_c.h"

struct C;
typedef void (C::*PMF)();

s32 dScMgHanachan_c::Behavior()
{
    char *c = (char *)this;

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
