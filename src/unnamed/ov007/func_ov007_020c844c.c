#include "Ov007Spring.h"
#include "nitro/fx/fx_cp.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c83fc(void *o);

/* Builds a follower that moves the Q12 position at p, with mass q (Q12):
 * invMass is its fixed-point reciprocal (FX_Inv), 0x1000 directly
 * for q = 1.0. */
Ov007Follower *func_ov007_020c844c(Vector3 *p, int q) {
    Ov007Follower *f = func_ov007_020c3df4(0, sizeof(Ov007Follower));
    func_ov007_020c83fc(f);
    f->target = p;
    if (q == 0x1000)
        f->invMass = 0x1000;
    else
        f->invMass = FX_Inv(q);
    return f;
}
