//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007Spring.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

/* Vector3 declares a constructor and destructor under C++, which makes its
 * assignment copy word by word. The ROM copies these with C's ldm/stm block
 * move, which C++ keeps for a struct with an array member. */
typedef struct Vec3Words {
    fx32 w[3];
} Vec3Words;


/* Pulls follower f toward target. Within snapRadius it snaps onto target,
 * its velocity and force cleared (returns 2). Within reach, a force of
 * strength pull along the offset is added (returns 1). Otherwise returns 0. */
int func_ov007_020c704c(Ov007Follower *f, Vector3 *target, int pull, int reach, int snapRadius) {
    int dx, dy, sum, spd, ratio;
    s64 sdx, sdy;

    dx = target->x - f->target->x;
    dy = target->y - f->target->y;
    sum = FX_MUL(dx, dx) + FX_MUL(dy, dy);
    sdx = dx;
    sdy = dy;

    if (sum <= FX_MUL(snapRadius, snapRadius)) {
        f->force.z = 0;
        f->force.y = f->force.z;
        f->force.x = f->force.y;
        f->velocity.z = f->force.x;
        f->velocity.y = f->velocity.z;
        f->velocity.x = f->velocity.y;
        *(Vec3Words *)f->target = *(Vec3Words *)target;
        return 2;
    }

    if (sum <= FX_MUL(reach, reach)) {
        spd = FX_Sqrt(sum);
        ratio = FX_Div(pull, spd);

        f->force.x += FX_MUL(ratio, sdx);
        f->force.y += FX_MUL(ratio, sdy);
        return 1;
    }
    return 0;
}
}
