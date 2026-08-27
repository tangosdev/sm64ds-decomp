//cpp
// @symbol _ZN7Minimap21FixTHIPaintingRoomPosER7Vector3
/* recovered: shared header, real C++ method (static)
 *
 * Bends a position inside the THI painting room so the minimap draws it in the
 * right place -- the room's real geometry and its map are not the same shape.
 *
 * It only fires on one specific place: level 0x1d, sublevel 5, room 2. Three
 * guards in a row, and any of them failing leaves the position untouched.
 *
 * Two zones, split at p0.z, each anchored on its own reference point and
 * scaled DIFFERENTLY per axis -- the far zone's x scale is even computed from
 * how far past the anchor the point is (`0xc00 - depth/-0x2e60`), so the
 * correction stretches with distance rather than being a fixed factor.
 *
 * No `this`: the ROM keeps r0 (the Vector3) in r4 and clobbers r1 before any
 * use, so the only incoming pointer is the argument. See include/Minimap.h.
 */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Minimap.h"

extern "C" {
extern void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern signed char data_0209f2f8;
}

void Minimap::FixTHIPaintingRoomPos(Vector3 & v_)
{
    struct Vector3* v = &v_;
    struct Vector3 p0, p1, out, out2;

    p0.x = (int)0xfee30000;
    p0.y = 0;
    p0.z = (int)0xff564000;
    p1.x = -0x1440000;
    p1.y = 0;
    p1.z = (int)0xff741000;

    if (SublevelToLevel(data_0209f2f8) != 0x1d) return;
    if (data_0209f2f8 != 5) return;
    if (data_ov002_02111148 != 2) return;

    if (v->z <= p0.z) {
        int factor;
        Vec3_Sub(&out, v, &p0);
        factor = 0xc00 - (_ZN4cstd4fdivEii(out.z, (int)0xffffd1a0) >> 12);
        v->x = p0.x + (int)(((long long)out.x * factor + 0x800) >> 12);
        v->z = p0.z + (int)(((long long)out.z * 0x200 + 0x800) >> 12);
    } else {
        if (v->x > p1.x) return;
        Vec3_Sub(&out2, v, &p1);
        v->x = p1.x + (int)(((long long)out2.x * 0x400 + 0x800) >> 12);
        v->z = p1.z + (int)(((long long)out2.z * 0xe00 + 0x800) >> 12);
    }
}
