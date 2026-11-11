#include "Ov007Spring.h"

/* out = b + s * a, per component in Q12. */
extern void VEC_MultAdd(int s, int *a, int *b, int *out);

/* One damped Euler step in x, y and z, the 3D form of func_ov007_020c81a0;
 * here the direct path replaces velocity with the damped force. */
void func_ov007_020c831c(Ov007Follower *f, int damping, int shift) {
    if ((f->flags & 1) == 0) {
        VEC_MultAdd(-damping, &f->velocity.x, &f->force.x, &f->force.x);
        if (f->invMass == 0x1000) {
            f->velocity.x = f->force.x >> shift;
            f->velocity.y = f->force.y >> shift;
            f->velocity.z = f->force.z >> shift;
        } else {
            VEC_MultAdd(f->invMass >> shift, &f->force.x, &f->velocity.x, &f->velocity.x);
        }
        f->target->x += f->velocity.x >> shift;
        f->target->y += f->velocity.y >> shift;
        f->target->z += f->velocity.z >> shift;
    }
    f->force.z = 0;
    f->force.y = f->force.z;
    f->force.x = f->force.y;
}
