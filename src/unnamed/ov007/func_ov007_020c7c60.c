/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "nitro/fx/fx.h"
#include "Ov007Spring.h"
extern void VEC_Subtract(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void VEC_Add(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);

/* func_ov007_020c7560's spring step that holds the link's rest offset: pulls
 * from - to towards dir with `stiffness` (into each end's force, opposite
 * signs), then damps each end's velocity by `damping`. */
void func_ov007_020c7c60(Ov007Link *link, int stiffness, int damping) {
    struct Vector3 d2, scaled, d1;
    int m, s;

    VEC_Subtract(link->from->target, link->to->target, &d1);
    VEC_Subtract(&d1, &link->dir, &d2);

    m = -stiffness;
    scaled.x = FX_MUL(m, d2.x);
    scaled.y = FX_MUL(m, d2.y);
    scaled.z = FX_MUL(m, d2.z);

    VEC_Add(&link->from->force, &scaled, &link->from->force);
    VEC_Subtract(&link->to->force, &scaled, &link->to->force);

    s = -damping;
    VEC_MultAdd(s, &link->from->velocity.x, &link->from->force.x, &link->from->force.x);
    VEC_MultAdd(s, &link->to->velocity.x, &link->to->force.x, &link->to->force.x);
}
