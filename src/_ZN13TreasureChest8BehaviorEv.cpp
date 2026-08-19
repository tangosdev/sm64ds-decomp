//cpp
// @symbol _ZN13TreasureChest8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TreasureChest.h"
struct dCc_c { int dummy; };
extern "C" {
extern void _ZN5dCc_c5ClearEv(struct dCc_c *t);
extern void _ZN5dCc_c6UpdateEv(struct dCc_c *t);
}

int TreasureChest::Behavior()
{
    func_ov064_0211a734(((char *)this));
    _ZN5dCc_c5ClearEv((struct dCc_c *)((char *)&mdCcAc_c));
    _ZN5dCc_c6UpdateEv((struct dCc_c *)((char *)&mdCcAc_c));
    return 1;
}
