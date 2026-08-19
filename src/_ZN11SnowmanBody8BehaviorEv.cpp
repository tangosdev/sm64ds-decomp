//cpp
// @symbol _ZN11SnowmanBody8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanBody.h"
struct dCc_c { void Clear(); void Update(); };
extern "C" void func_ov072_0211f3e4(void *c);

int SnowmanBody::Behavior()
{
    func_ov072_0211fc3c(((char *)this));
    ((dCc_c*)((char *)&mdCcAc_c))->Clear();
    ((dCc_c*)((char *)&mdCcAc_c))->Update();
    func_ov072_0211f3e4(((char *)this));
    return 1;
}
