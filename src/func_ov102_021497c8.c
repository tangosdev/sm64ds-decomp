// @symbol func_ov102_021497c8
// @emits QuestionBlock_OnAttacked1
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjHatenaBlock_c::OnAttacked1 - recovered from vtable slot identity */

void QuestionBlock_OnAttacked1(void* self, void* arg) {
    int v = *(int*)((char*)self + 0x3e8);
    if (v == 1) return;
    if (func_ov102_02149078(self)) return;
    int val = *(int*)((char*)arg + 8);
    *(unsigned char*)((char*)self + 0x3f2) = val;
    func_ov102_02149da8(self, 1);
}
