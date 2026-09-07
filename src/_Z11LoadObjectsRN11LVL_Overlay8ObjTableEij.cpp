//cpp
// @symbol _Z11LoadObjectsRN11LVL_Overlay8ObjTableEij
/* LoadObjects(LVL_Overlay::ObjTable&, int, u32) at ov002:0x020fe33c -- walk the
 * outer object table and dispatch each 8-byte entry to the category loader
 * table at data_ov002_0210cbb8.
 *
 * A free function, not a member: no `this`, and the mangled name has no class
 * prefix. The table type is nested, which is why include/LVL_Overlay.h declares
 * ObjTable inside LVL_Overlay rather than beside it -- that nesting is what
 * makes the compiler emit `RN11LVL_Overlay8ObjTableE`.
 *
 * Count is a u16 at offset 0 (unlike ObjSubTable's u8 at offset 1). Each entry
 * is 8 bytes; byte 0 packs a filter in the top three bits and a handler index
 * in the low five. The entry is skipped unless the filter is 0 or equal to the
 * global byte at data_0209f220. The handler is then looked up and called if
 * non-null. */
#include "types.h"
#include "LVL_Overlay.h"

extern "C" {
extern void (*data_ov002_0210cbb8[])(void*, int, unsigned int);
extern u8 data_0209f220;
}

void LoadObjects(LVL_Overlay::ObjTable& t, int areaID, u32 param)
{
    u8* e = (u8*)t.entries;
    int i;
    for (i = 0; i < (int)t.count; i++) {
        u8 b = *e;
        int type = (b >> 5) & 7;
        if (type == 0 || type == data_0209f220) {
            void (*h)(void*, int, unsigned int) = data_ov002_0210cbb8[b & 0x1f];
            if (h != 0)
                h(e, areaID, param);
        }
        e += 8;
    }
}
