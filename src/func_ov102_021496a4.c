#include "types.h"
// @symbol func_ov102_021496a4
// @emits QuestionBlock_OnHitFromUnderneath
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjHatenaBlock_c::OnHitFromUnderneath - recovered from vtable slot identity */
void QuestionBlock_OnHitFromUnderneath(void *c, void *x) {
    if (*(int *)((char *)c + 0x3e8) == 1) return;
    *(int *)((char *)c + 0x9c) = -0x8000;
    *(int *)((char *)c + 0xa8) = 0x1e000;
    if (func_ov102_02149078(c)) return;
    *(u8 *)((char *)c + 0x3f2) = *(int *)((char *)x + 8);
    func_ov102_02149da8(c, 1);
}
