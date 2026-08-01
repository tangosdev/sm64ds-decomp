// @symbol func_ov060_02118728
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#pragma opt_strength_reduction off
#pragma opt_common_subs off



extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern int Vec3_Dist(struct Vector3 *a, struct Vector3 *b);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);

void func_ov060_02118728(char *c)
{
    struct Vector3 v;
    void *player;
    char *bestActor;
    char *actor;
    int best;
    int i;

    player = _ZN5Actor13ClosestPlayerEv(c);
    if (((*(int *)(c + 0xb0) & 8) ? 1 : 0) == 0) return;
    if (player == 0) return;

    {
        struct Vector3 *pp = (struct Vector3 *)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFFULL);
        v.x = pp->x;
        v.y = pp->y;
        v.z = pp->z;
    }
    best = Vec3_Dist((struct Vector3 *)(c + 0x5c), &v);
    bestActor = c;

    for (i = 0; i < 8; i++) {
        int id = *(int *)(c + (i << 2) + 0x188);
        if (id == 0) continue;
        actor = (char *)_ZN5Actor10FindWithIDEj(id);
        if (actor != 0) {
            if (*(int *)(actor + 0x170) != 3) return;
            if (((*(int *)(actor + 0xb0) & 8) ? 1 : 0) == 0) continue;
            {
                int d = Vec3_Dist(&v, (struct Vector3 *)(actor + 0x5c));
                if (d < best) {
                    best = d;
                    bestActor = actor;
                }
            }
        } else {
            *(int *)(c + (i << 2) + 0x188) = 0;
        }
    }
    func_ov060_021186d8(bestActor);
}
