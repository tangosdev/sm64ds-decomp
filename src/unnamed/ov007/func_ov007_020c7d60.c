#include "types.h"
#include "nitro/fx/fx.h"

/* SubVec3 still defines its vector as its own struct Vec3;
   declared as defined, the call casts. */
struct Vec3;
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern void func_02053320(int s, int *a, int *b, int *out);
extern int DotVec3(const Vector3 *a, const Vector3 *b);

/* The 0x24-byte point func_ov007_020c844c allocates; names as in
 * func_ov007_020c83fc, only the slots this function touches. */
typedef struct Ov007Follower {
    int *target;     /* 0x00: the Q12 position each step moves */
    int velocity[3]; /* 0x04 */
    int force[3];    /* 0x10: accumulated per frame */
} Ov007Follower;

/* A spring between two points; names as in func_ov007_020c6314, only the
 * slots this function reads. */
typedef struct Ov007Link {
    Ov007Follower *from; /* 0x0 */
    Ov007Follower *to;   /* 0x4 */
    int length;          /* 0x8: the rest length */
} Ov007Link;

typedef char
    Ov007Follower_force_must_be_at_0x10[(int)&((Ov007Follower *)0)->force == 0x10 ? 1 : -1];

/* func_ov007_020c7560's spring step that holds the link's rest length: adds
 * -r5 times the stretch along the unit from - to direction into each end's
 * force (opposite signs), then damps each end's velocity along that
 * direction by r4. self keeps the char ** spelling of the definition;
 * `link` is the typed view. */
void func_ov007_020c7d60(char **self, int r5, int r4)
{
    Ov007Link *link = (Ov007Link *)self;
    Vector3 d;
    s64 sq;
    volatile u16 *ime;
    u16 saved;
    int sqrt_raw, len, dot, s, rest, *p_sqrt;
    Vector3 v1, v2;
    int delta;

    SubVec3((struct Vec3 *)link->from->target, (struct Vec3 *)link->to->target, (struct Vec3 *)&d);

    /* x-first in source; mwccarm emits y-first loads (same as Vec3_DistSq) */
    sq = (s64)(d.x) * (d.x);
    sq += (s64)(d.y) * (d.y);
    sq += (s64)(d.z) * (d.z);

    ime = (volatile u16 *)0x4000208;
    saved = *ime;
    *ime = 0;
    *(volatile u16 *)0x40002b0 = 1;
    *(volatile u64 *)0x40002b8 = (u64)sq << 2;
    *ime;
    *ime = saved;

    *(volatile u16 *)0x4000280 = 2;
    *(volatile u64 *)0x4000290 = (u64)0x1000000 << 32;
    *(volatile u64 *)0x4000298 = (u64)sq;

    while (*(volatile u16 *)0x40002b0 & 0x8000) {}

    p_sqrt = (int *)0x40002b4;
    rest = link->length;
    sqrt_raw = *p_sqrt;
    len = (sqrt_raw + 1) / 2;

    delta = len - rest;
    r5 = FX_MUL(delta, r5);
    r5 = -r5;

    while (*(volatile u16 *)0x4000280 & 0x8000) {}

    {
        s64 t = (s64)sqrt_raw * *(volatile s64 *)0x40002a0;
        d.x = (int)((t * d.x + (1LL << 44)) >> 45);
        d.y = (int)((t * d.y + (1LL << 44)) >> 45);
        d.z = (int)((t * d.z + (1LL << 44)) >> 45);
    }

    func_02053320(r5, (int *)&d, link->from->force, link->from->force);
    func_02053320(-r5, (int *)&d, link->to->force, link->to->force);

    dot = DotVec3(&d, (Vector3 *)link->from->velocity);
    v1.x = (int)(((s64)dot * d.x) >> 12);
    v1.y = (int)(((s64)dot * d.y) >> 12);
    v1.z = (int)(((s64)dot * d.z) >> 12);

    dot = DotVec3(&d, (Vector3 *)link->to->velocity);
    v2.x = (int)(((s64)dot * d.x) >> 12);
    v2.y = (int)(((s64)dot * d.y) >> 12);
    v2.z = (int)(((s64)dot * d.z) >> 12);

    s = -r4;
    func_02053320(s, (int *)&v1, link->from->force, link->from->force);
    func_02053320(s, (int *)&v2, link->to->force, link->to->force);
}
