// @symbol func_ov100_02143aa4
// recovered name: RollingIronBall_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daIbl_c::Kill - recovered from vtable slot identity */
extern void _ZN9Animation7AdvanceEv(void *);
int func_ov100_02143aa4(char *c)
{
    *(int *)(c + 0x368) = 4096;
    _ZN9Animation7AdvanceEv((char *)c + 0x35c);
    func_ov100_021437d4(c);
    func_ov100_0214344c(c);
    func_ov100_021435e8(c);
    return 1;
}
