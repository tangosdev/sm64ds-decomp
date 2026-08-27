//cpp
// @symbol _ZN12dScMgSound_c6RenderEv
#include "decl_common.h"
#include "dScMgSound_c.h"
/* dScMgSound_c::Render -- vtable slot 9, ov006 0x0211c6c4.
 *
 * Attributed by the vtable: dScMgSound_c's own table is ov006 0x0213f844 and
 * its slot 9 relocates here, so this is this class's own override and not one
 * it inherits from dScMgSingle3DBase_c. The signature is
 * include/dScMgBase_c.h's own slot 9, `virtual s32 Render()`.
 *
 * Five of the six calls take the scene itself; the last takes mTable, the one
 * member this class owns, at 0x4f38. */

extern "C" void func_ov004_020b1e34(void *c, int a, int b, int d);

s32 dScMgSound_c::Render()
{
    char *c = (char *)this;

    func_ov004_020b1e34(c, 0xe0, 0x14, 1);
    func_ov006_02119c74(c);
    func_ov006_02119bdc(c);
    func_ov006_02119bc4(c);
    func_ov006_021199c0(c);
    func_ov006_02119aa8(c);
    func_ov006_020c29dc(mTable);
    return 1;
}
