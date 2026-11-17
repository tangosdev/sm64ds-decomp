#include "nitro/fx/fx.h"
#include "nitro/fx/fx_vec.h"
#include "Ov007Spring.h"

/* func_ov007_020c7560's spring step that holds the link's rest offset: pulls
 * from - to towards dir with `stiffness` (into each end's force, opposite
 * signs), then damps each end's velocity by `damping`. */
void func_ov007_020c7c60(Ov007Link *link, int stiffness, int damping) {
    VecFx32 d2, scaled, d1;
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
    VEC_MultAdd(s, &link->from->velocity, &link->from->force, &link->from->force);
    VEC_MultAdd(s, &link->to->velocity, &link->to->force, &link->to->force);
}
