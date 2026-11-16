#include "nitro/fx/fx_vec.h"
#include "Ov007Spring.h"

/* One damped Euler step in x, y and z, the 3D form of func_ov007_020c81a0;
 * here the direct path replaces velocity with the damped force. */
void func_ov007_020c831c(Ov007Follower *f, int damping, int shift) {
    if ((f->flags & 1) == 0) {
        VEC_MultAdd(-damping, &f->velocity, &f->force, &f->force);
        if (f->invMass == 0x1000) {
            f->velocity.x = f->force.x >> shift;
            f->velocity.y = f->force.y >> shift;
            f->velocity.z = f->force.z >> shift;
        } else {
            VEC_MultAdd(f->invMass >> shift, &f->force, &f->velocity, &f->velocity);
        }
        f->target->x += f->velocity.x >> shift;
        f->target->y += f->velocity.y >> shift;
        f->target->z += f->velocity.z >> shift;
    }
    f->force.z = 0;
    f->force.y = f->force.z;
    f->force.x = f->force.y;
}
