// @symbol func_ov007_020cc028
/* recovered: vtable identified, declarations from a shared header */
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV9dScDSMT_c; VT1 = _ZTV8dScene_c; VT2 = _ZTV7dBase_c */
extern void _ZN9ActorBaseD2Ev(void *);
int *func_ov007_020cc028(int *t)
{
    t[0] = (int)_ZTV9dScDSMT_c;
    func_02017254((char *)t + 0x54);
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN9ActorBaseD2Ev(t);
    return t;
}
