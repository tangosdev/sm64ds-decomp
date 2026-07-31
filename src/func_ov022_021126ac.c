// @symbol func_ov022_021126ac
// @emits daObjFlMaruta_c_AfterClsn
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjFlMaruta_c::AfterClsn - recovered from vtable slot identity */
extern int _ZN9ActorBase18MarkForDestructionEv(void *c);
int daObjFlMaruta_c_AfterClsn(char *c) {
    int a = *(int*)(c+0x60);
    int b = *(int*)(c+0x118);
    if (a < b) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }
    unsigned short h = *(unsigned short*)(c+0x100+0x10);
    if (h == 0) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }
    data_ov025_02112654(c);
    return 1;
}
