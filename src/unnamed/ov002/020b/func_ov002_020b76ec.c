#include "types.h"
// @symbol func_ov002_020b76ec
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern u8 data_0209f2d8[];
extern void* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 a, const struct Vector3 *p);
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param1, const struct Vector3* pos, const struct Vector3_16* rot,
    int areaID, int deathTableID);

int func_ov002_020b76ec(char *self)
{
    *(int *)(self + 0xa4) = 0;
    *(int *)(self + 0xa8) = 0;
    *(int *)(self + 0xac) = 0;
    *(int *)(self + 0x9c) = 0;
    *(u8 *)(self + 0x401) = 0;
    *(int *)(self + 0x3ec) = 0;
    *(int *)(self + 0xc8) = 0;
    *(u8 *)(self + 0x403) = 1;

    *(u32 *)(((int)self + 0xb0)) &= ~0x40000u;
    *(u32 *)(((int)self + 0x12c)) &= ~0x8000u;

    if (*(int *)(self + 0x3c0) == 0) {
        *(void **)(self + 0x3c0) = _ZN8dActor_c13ClosestPlayerEv(self);
    }

    {
        unsigned b = (data_0209f2d8[0] == 1);
        if (b == 0) {
            char *p = *(char **)(self + 0x3c0);
            if (*(u8 *)(p + 0x6d9) == 3) {
                if (*(int *)(self + 0x3f4) == *(int *)(p + 8)) {
                    struct Vector3 v;
                    _ZN5Sound9PlayBank3EjRK7Vector3(0x6e, (struct Vector3 *)(self + 0x74));
                    GiveLives(1);
                    v.x = *(int *)(self + 0x5c);
                    v.y = *(int *)(self + 0x60);
                    v.z = *(int *)(self + 0x64);
                    v.y += 0x64000;
                    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x14b, 8, &v, (struct Vector3_16 *)0, *(signed char *)(self + 0xcc), -1);
                }
            }
        }
    }

    *(u16 *)(self + 0x100) = 0x96;
    *(int *)(self + 0x80) = 0x1000;
    *(int *)(self + 0x84) = 0x1000;
    *(int *)(self + 0x88) = 0x1000;
    return 1;
}
