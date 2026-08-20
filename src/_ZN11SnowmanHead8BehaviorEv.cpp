//cpp
// @symbol _ZN11SnowmanHead8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanHead.h"
struct dCc_c { void Clear(); void Update(); };
extern "C" void func_ov072_0211ffd8(void *c);

int SnowmanHead::Behavior()
{
    func_ov072_02120560(((char *)this));
    ((dCc_c*)((char *)&mdCcAc_c))->Clear();
    ((dCc_c*)((char *)&mdCcAc_c))->Update();
    func_ov072_0211ffd8(((char *)this));
    return 1;
}
