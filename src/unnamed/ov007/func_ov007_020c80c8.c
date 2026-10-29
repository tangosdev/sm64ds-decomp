#include "types.h"
#include "nitro/fx/fx.h"

/* The 0x24-byte follower record func_ov007_020c844c allocates; names as in
 * func_ov007_020c83fc's view (from its steppers func_ov007_020c81a0 and
 * func_ov007_020c831c), which this file's z-only step reads the same way:
 * it adds force to velocity and velocity to target, then clears force. */
typedef struct Ov007Follower {
    int *target;     /* 0x00: the Q12 position each step moves */
    int velocity[3]; /* 0x04: added to target[] every step, >> shift */
    int force[3];    /* 0x10: accumulated per frame, cleared at the end of each step */
    /* 0x1c: Q12 reciprocal of func_ov007_020c844c's q; 0x1000 takes the
     * direct path. Every stepper loads it signed (ldrsh), so s16 here. */
    s16 invMass;
    u16 pad_1e;
    int flags; /* 0x20: bit 0 skips the step (target held) */
} Ov007Follower;

typedef char Ov007Follower_size_must_be_0x24[sizeof(Ov007Follower) == 0x24 ? 1 : -1];

/* One damped Euler step on the z axis: a is the damping factor, s the
 * step shift. */
void func_ov007_020c80c8(char *o, int a, int s)
{
    Ov007Follower *f = (Ov007Follower *)o;
    if (!(f->flags & 1)) {
        if (f->invMass == 0x1000) {
            f->velocity[2] += (f->force[2] - FX_MUL(a, f->velocity[2])) >> s;
        } else {
            f->force[2] -= FX_MUL(a, f->velocity[2]);
            f->velocity[2] += FX_MUL(f->force[2], (f->invMass >> s));
        }
        f->target[2] += f->velocity[2] >> s;
    }
    f->force[2] = 0;
}
