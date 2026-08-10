//cpp
// @symbol _Z17LoadSimpleObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadSimpleObjects(LVL_Overlay::ObjSubTable&, int, u32) at ov002:0x020fe960
 * -- spawn simple actors, or hand a minimap-change sentinel to
 * LoadMinimapChangeObject.
 *
 * A free function taking a nested LVL_Overlay::ObjSubTable reference. Each
 * SimpleEntry is 8 bytes: a packed `raw' word (low 9 bits = actor-table index,
 * high bits = param) and an s16 position. There is no rotation; spawn passes
 * NULL for it. raw == 0x1ff is the minimap-change special case.
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

void _Z23LoadMinimapChangeObjecti5Fix12IiEh(int a, Fix12i b, s8 h);
/* Real symbol ends in `as` (s8 area, s16 deathTable) -- not `ii`. Call-site
   bytes are insensitive to the declared widths; the linker is not. */
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param1, const Vector3* pos, const Vector3s* rot,
    s32 areaID, s32 deathTableID);
}

void LoadSimpleObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::SimpleEntry* e = (LVL_Overlay::SimpleEntry*)tbl.entries;
    int i;
    for (i = 0; i < (int)tbl.count; i++, e++) {
        Vector3 v;
        int raw;
        int actorParam;
        int masked;
        int zz = e->z << 12;
        int yy = e->y << 12;
        int xx = e->x << 12;
        v.x = xx;
        v.y = yy;
        v.z = zz;
        raw = e->raw;
        masked = (u16)(raw & 0x1ff);
        actorParam = raw >> 9;
        if (masked == 0x1ff) {
            _Z23LoadMinimapChangeObjecti5Fix12IiEh(areaID, yy, (s8)(actorParam & 0xf));
        } else {
            s16 old = data_ov002_0211118c;
            data_ov002_0211118c = (s16)(old + 1);
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                data_ov002_0210cbf4[masked], actorParam, &v, 0, areaID, old);
        }
    }
}
