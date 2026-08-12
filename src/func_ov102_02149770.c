#include "types.h"
// @symbol func_ov102_02149770
// recovered name: QuestionBlock_OnKicked
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjHatenaBlock_c::OnKicked
 * func_ov102_02149770 at 0x02149770
 * Byte-matched with mwccarm 2004/b56 (ov102), strict-reloc verified.
 */
void func_ov102_02149770(void *c, void *x) {
    if (*(int *)((char *)c + 0x3e8) == 1) return;
    int r = func_ov102_02149078(c);
    if (r != 0) return;
    *(u8 *)((char *)c + 0x3f2) = *(int *)((char *)x + 8);
    func_ov102_02149da8(c, 1);
}
