//cpp
// @symbol _ZN14dScMgMemory2_c6RenderEv
#include "decl_common.h"
#include "dScMgMemory2_c.h"
/* dScMgMemory2_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the third of the three slots where this class's
 * table differs from dScMgSingle3DBase_c's. Three draw passes over the card field
 * plus the shared HUD, in the fixed order the ROM has.
 *
 * WAS A C99 FILE, so the three declarations move inside `extern "C"`; in C++ they
 * would mangle and resolve to nothing. */

extern "C" {
void func_ov006_020c0aa8(char *);
void func_ov004_020b1bc8(char *, int, int, int);
void func_ov006_020c1804(char *);
}

s32 dScMgMemory2_c::Render()
{
    char *c = (char *)this;
    func_ov006_020c0aa8(c + 0x4660);
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();
    DrawMessage();
    DrawCursor();
    DrawCards();
    func_ov006_020c1804(c + 0x4f38);
    return 1;
}
