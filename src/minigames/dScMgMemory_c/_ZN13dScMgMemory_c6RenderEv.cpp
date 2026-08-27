//cpp
// @symbol _ZN13dScMgMemory_c6RenderEv
#include "decl_common.h"
#include "dScMgMemory_c.h"
/* dScMgMemory_c::Render -- vtable slot 9, ov006 0x020f5324.
 *
 * Attributed by the vtable: dScMgMemory_c's own table is ov006 0x0213d1b8 --
 * the address include/dScMgMemory_c.h's banner uses to prove the class is not
 * the "MgMemoryMatch" its destructor pair was once named after -- and its slot
 * 9 relocates here. The signature is include/dScMgBase_c.h's own slot 9,
 * `virtual s32 Render()`.
 *
 * The two offsets that are not members are the two sub-objects the class does
 * not own: +0x4660 belongs to dScMgSingle3DBase_c and +0x4f38 is the 0x270
 * shared table five siblings use, which this class's methods never read
 * inside. Both are passed as base addresses to their own helpers. */

extern "C" {
void func_ov006_020c0aa8(char *p);
void func_ov004_020b1bc8(char *c, int a, int b, int d);
void func_ov006_020c1804(char *p);
}

s32 dScMgMemory_c::Render()
{
    char *c = (char *)this;

    func_ov006_020c0aa8(c + 0x4660);
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();
    func_ov006_020f38f0(c);
    func_ov006_020f392c(c);
    func_ov006_020f3e68(c);
    func_ov006_020c1804(c + 0x4f38);
    return 1;
}
