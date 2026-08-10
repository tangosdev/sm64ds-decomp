//cpp
// @symbol _Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadStandardObjects(LVL_Overlay::ObjSubTable&, int, u32) at ov002:0x020fe8ac
 * -- spawn one actor per standard object entry.
 *
 * A free function taking a nested LVL_Overlay::ObjSubTable reference. Each
 * StandardEntry is 0x10 bytes: raw id index into data_ov002_0210cbf4, s16
 * position (shifted left by 12 into Fix12), Vector3s rotation, u16 param.
 * data_ov002_0211118c is a running death-table / spawn-order counter incremented
 * per spawn.
 *
 * The third parameter is part of the shared loader signature and unused here.
 *
 * The file this replaces declared its own Entry/ObjSubTable/Vec types and
 * hand-spelled the mangled symbol inside an extern "C" block. */
#include "types.h"
#include "LVL_Overlay.h"

extern "C" {
extern s16 data_ov002_0211118c;
extern u16 data_ov002_0210cbf4[];

/* Real symbol ends in `as` (s8 area, s16 deathTable) -- not `ii`. Call-site
   bytes are insensitive to the declared widths; the linker is not. */
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param1, const Vector3* pos, const Vector3s* rot,
    s32 areaID, s32 deathTableID);
}

void LoadStandardObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::StandardEntry* e = (LVL_Overlay::StandardEntry*)tbl.entries;
    int i;
    for (i = 0; i < (int)tbl.count; i++, e++) {
        Vector3 v;
        u32 a = data_ov002_0210cbf4[e->raw];
        int zz = e->z << 12;
        int yy = e->y << 12;
        int xx = e->x << 12;
        v.x = xx;
        v.y = yy;
        v.z = zz;
        s16 old = data_ov002_0211118c;
        data_ov002_0211118c = (s16)(old + 1);
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
            a, e->param, &v, &e->rot, areaID, old);
    }
}
