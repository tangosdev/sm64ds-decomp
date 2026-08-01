// @symbol func_ov102_02149710
// recovered name: QuestionBlock_OnHitByMegaChar
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjHatenaBlock_c::OnHitByMegaChar - recovered from vtable slot identity */
extern void _ZN6Player16IncMegaKillCountEv(void *p);
void func_ov102_02149710(void *c, void *player)
{
    if (*(int*)((char*)c+0x3e8) == 1) return;
    if (func_ov102_02149078(c) != 0) return;
    _ZN6Player16IncMegaKillCountEv(player);
    *(unsigned char*)((char*)c+0x3f2) = *(int*)((char*)player+8);
    func_ov102_02149da8(c, 1);
}
