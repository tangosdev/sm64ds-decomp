//cpp
// @symbol _Z15LoadExitObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadExitObjects(LVL_Overlay::ObjSubTable&, int, u32) at ov002:0x020fe420 --
 * spawn one exit actor (id 0x15d) per entry.
 *
 * Positions are stored as s16 world units and shifted left by 12 into Fix12,
 * which is what every loader in this family does. Rotations are stored the same
 * way and NEGATED -- both of them -- so a stored angle is the opposite of the
 * one the actor gets.
 *
 * THE PARAMETER WORD IS STORED BIG-ENDIAN. Bytes 0x0a..0x0d assemble as
 * `(b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]', which on a little-endian
 * machine is the reverse of a plain load. That is not an artefact of the
 * recovery -- the ROM really does shift 0x0a by 24 -- so the field stays a
 * four-byte array in the header rather than being declared a u32 that would read
 * backwards.
 *
 * The entry stride is 0xe, which is the record's size and is now asserted at
 * compile time in the header rather than living as a magic `e += 0xe'.
 *
 * This file used to declare Vector3, Vector3_16, its own typedefs for u32/s32/
 * s16/u8, and reach the table through `char* t' with `*(u8*)(t + 1)' for the
 * count and `*(char**)(t + 4)' for the entries. */
#include "types.h"
#include "LVL_Overlay.h"

extern "C" void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param1, const Vector3* pos, const Vector3s* rot,
    s32 areaID, s32 deathTableID);

void LoadExitObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::ExitEntry* e = (LVL_Overlay::ExitEntry*)tbl.entries;

    for (int i = 0; i < (int)tbl.count; i++)
    {
        Vector3 pos;
        s32 pz = e->z << 12;
        s32 py = e->y << 12;
        s32 px = e->x << 12;
        pos.x = px;
        pos.y = py;
        pos.z = pz;

        Vector3s rot;
        rot.x = -e->rotX;
        rot.y = -e->rotY;

        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x15d,
            ((u32)e->param[0] << 24) | ((u32)e->param[1] << 16)
                | ((u32)e->param[2] << 8) | (u32)e->param[3],
            &pos, &rot, -1, -1);

        e++;
    }
}
