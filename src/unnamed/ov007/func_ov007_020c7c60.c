// @symbol func_ov007_020c7c60
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "nitro/fx/fx.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);

/* The 0x24-byte point func_ov007_020c844c allocates; names as in
 * func_ov007_020c83fc, only the slots this function touches. */
typedef struct Ov007Follower {
    int *target;     /* 0x00: the Q12 position each step moves */
    int velocity[3]; /* 0x04 */
    int force[3];    /* 0x10: accumulated per frame */
} Ov007Follower;

/* A spring between two points; names as in func_ov007_020c6314. */
typedef struct Ov007Link {
    Ov007Follower *from; /* 0x0 */
    Ov007Follower *to;   /* 0x4 */
    int length;          /* 0x8 */
    struct Vector3 dir;  /* 0xc: the rest offset from `to` to `from` */
} Ov007Link;

typedef char Ov007Link_size_must_be_0x18[sizeof(Ov007Link) == 0x18 ? 1 : -1];

/* func_ov007_020c7560's spring step that holds the link's rest offset: pulls
 * from - to towards dir with stiffness r6 (into each end's force, opposite
 * signs), then damps each end's velocity by r4. self keeps the char **
 * spelling of the definition; `link` is the typed view. */
void func_ov007_020c7c60(char **self, int r6, int r4)
{
    Ov007Link *link = (Ov007Link *)self;
    struct Vector3 d2, scaled, d1;
    int m, s;

    SubVec3((struct Vector3 *)link->from->target, (struct Vector3 *)link->to->target, &d1);
    SubVec3(&d1, &link->dir, &d2);

    m = -r6;
    scaled.x = FX_MUL(m, d2.x);
    scaled.y = FX_MUL(m, d2.y);
    scaled.z = FX_MUL(m, d2.z);

    AddVec3((struct Vector3 *)link->from->force, &scaled, (struct Vector3 *)link->from->force);
    SubVec3((struct Vector3 *)link->to->force, &scaled, (struct Vector3 *)link->to->force);

    s = -r4;
    func_02053320(s, link->from->velocity, link->from->force, link->from->force);
    func_02053320(s, link->to->velocity, link->to->force, link->to->force);
}
