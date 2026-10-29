#include "types.h"
#include "nitro/fx/fx.h"

/* Reconstructed layouts and names, not recovered class identity; only the
 * slots this file touches.
 *
 * Ov007Follower is the 0x24-byte point func_ov007_020c83fc clears (names as
 * in that file, PR #4236). */
typedef struct Ov007Follower {
    int *target;     /* 0x00: the Q12 position each step moves (func_ov007_020c844c's p) */
    int velocity[3]; /* 0x04: added to target[] every step, >> shift */
    int force[3];    /* 0x10: accumulated per frame, cleared at the end of each step */
} Ov007Follower;

/* A link between two points; names as in func_ov007_020c6314. In the spring
 * modes (func_ov007_020c7b2c x/y, this file z) dir is the rest offset of
 * from relative to to. */
typedef struct Ov007Link {
    Ov007Follower *from; /* 0x0 */
    Ov007Follower *to;   /* 0x4 */
    int length;          /* 0x8 */
    struct Vector3 dir;  /* 0xc */
} Ov007Link;

typedef char Ov007Follower_size_must_be_0x1c[sizeof(Ov007Follower) == 0x1c ? 1 : -1];
typedef char Ov007Link_size_must_be_0x18[sizeof(Ov007Link) == 0x18 ? 1 : -1];

/* func_ov007_020c7560's mode 3: a damped spring along z only. The stretch
 * past dir.z, times stiffness (Q12), pushes the two ends apart, and each end
 * is damped by its own z velocity times damping. p keeps the char * spelling
 * of the definition; `link` is the typed view. */
void func_ov007_020c7a84(char *p, int stiffness, int damping) {
    Ov007Link *link = (Ov007Link *)p;
    Ov007Follower *a = link->from;
    int t = a->target[2] - link->to->target[2] - link->dir.z;
    int m1 = FX_MUL(stiffness, t);
    int m2 = FX_MUL(damping, a->velocity[2]);
    int nm1 = -m1;
    a->force[2] += nm1 - m2;
    {
        Ov007Follower *b = link->to;
        int m3 = FX_MUL(damping, b->velocity[2]);
        b->force[2] -= nm1 + m3;
    }
}
