//cpp
// @symbol _Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadEntranceObjects(LVL_Overlay::ObjSubTable&, int, u32) at ov002:0x020fe6c8
 * -- spawn entrance actors and the entrance-controller dBase_c.
 *
 * A free function taking a nested LVL_Overlay::ObjSubTable reference. Each
 * StandardEntry is 0x10 bytes: raw id index, s16 position, Vector3s rotation,
 * u16 param. Positions shift left by 12 into Fix12 on the way to dActor_c::Spawn.
 *
 * The third argument is an entry index offset into the table (not a free
 * parameter like the sibling loaders' unused `param'): the walk starts at
 * `entries + p3' and runs for `data_0209f21c' iterations, not `tbl.count'.
 *
 * dActor_c::Spawn and dBase_c::Spawn are real static methods; mwccarm mangles
 * the call relocs (including dActor_c::Spawn's s8/s16 trailing args). */
#include "types.h"
#include "LVL_Overlay.h"
#include "dActor_c.h"

extern "C" {
extern u8 data_0209f21c;
extern u8 data_0209caa0[];
extern u8 data_0209f2d8;
extern u8 data_02092128[];
extern u16 data_ov002_0210cbf4[];
extern u8 data_0209fc5c[];
extern void* data_0209f394[];
extern u8 data_0209f250;
extern void* data_0209f5c0;
extern void* data_0209f318;
extern s8 data_ov002_0210cb5c[];

void func_0202b0e0(LVL_Overlay::StandardEntry* e, int count);
void StartEntranceFaderWipe(void);
}

void LoadEntranceObjects(LVL_Overlay::ObjSubTable& tbl, int p2, u32 p3)
{
    u32 sl;
    LVL_Overlay::StandardEntry* e = (LVL_Overlay::StandardEntry*)tbl.entries;
    func_0202b0e0(e, tbl.count);
    e += p3;
    sl = 0;

    int entranceId = 0xf;
    u8 i = 0;

    while (i < data_0209f21c) {
        {
            Vector3 pos;
            int vz = e->z << 12;
            int vy = e->y << 12;
            int vx = e->x << 12;
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;

            u16 param = e->param;
            sl = (param >> 7) & 0xf;

            int f2 = data_0209caa0[0x41];
            int f1 = data_02092128[i];
            int cond = (data_0209f2d8 == 1);
            if (cond != 0) {
                f2 = 3;
                f1 = 3;
            }
            u32 flags = f2 | (f1 << 3) | (i << 6) | (sl << 8);

            void* a = dActor_c::Spawn(
                data_ov002_0210cbf4[e->raw], flags, pos,
                (const Vector3_16*)&e->rot, (s8)(param & 7), -1);

            if (data_0209fc5c[i] == 0)
                data_0209f394[i] = 0;
            else
                data_0209f394[i] = a;

            if (i == data_0209f250)
                entranceId = (e->param >> 3) & 0xf;

            e++;
            i++;
        }
    }

    data_0209f318 = dBase_c::Spawn(
        0x14c, (fBase_c*)data_0209f5c0, entranceId, 0);

    int t = data_0209f2d8;
    t = t == 2;
    if (t != 0)
        return;

    if (sl >= 0x13)
        sl = 0;
    if (data_ov002_0210cb5c[sl] < 0)
        return;

    StartEntranceFaderWipe();
}
