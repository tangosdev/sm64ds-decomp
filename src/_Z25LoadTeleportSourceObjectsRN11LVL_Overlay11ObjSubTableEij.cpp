//cpp
// @symbol _Z25LoadTeleportSourceObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadTeleportSourceObjects(LVL_Overlay::ObjSubTable&, int, u32) at
 * ov002:0x020fe5f4 -- spawn one teleport-source actor (id 0x15b) per entry.
 *
 * Simpler than its exit-loader sibling in two ways that are visible in the
 * record: the parameter is a plain u16 rather than a big-endian byte quartet,
 * and there is no rotation at all -- the spawn call passes NULL for it, so a
 * teleport source has a position and nothing else. The record is 8 bytes against
 * the exit record's 0xe.
 *
 * The loop is bottom-tested with a guard (`if (count <= 0) return;' then
 * do/while) rather than top-tested, and that shape is load-bearing: it is what
 * the ROM emits, and a plain `for' reorders the compare.
 *
 * On the spawn declaration: the symbol says `PK10Vector3_16', but the call is
 * extern "C" so the name is fixed and the parameter type is this file's choice.
 * types.h calls that same three-short struct Vector3s, and it is spelled that
 * way here rather than re-declaring a second name for one type.
 *
 * The file this replaces declared its own `struct Entry', its own ObjSubTable,
 * and Vector3/Vector3_16 alongside them. */
#include "types.h"
#include "LVL_Overlay.h"

extern "C" void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param1, const Vector3* pos, const Vector3s* rot,
    s32 areaID, s32 deathTableID);

void LoadTeleportSourceObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::TeleportSourceEntry* e =
        (LVL_Overlay::TeleportSourceEntry*)tbl.entries;

    s32 i = 0;
    if ((int)tbl.count <= 0)
        return;

    do
    {
        Vector3 pos;
        s32 vz = e->z << 12;
        s32 vy = e->y << 12;
        s32 vx = e->x << 12;
        pos.x = vx;
        pos.y = vy;
        pos.z = vz;

        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x15b, e->param, &pos, (const Vector3s*)0, 0, -1);

        i++;
        e++;
    } while (i < (int)tbl.count);
}
