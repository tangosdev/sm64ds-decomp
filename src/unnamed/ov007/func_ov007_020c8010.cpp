//cpp
#include "Ov007Spring.h"
#include "math/CalcLib.h"

extern "C" {

/* Links two followers: once both have a target, dir is the vector between
 * them and length its length. */
void func_ov007_020c8010(Ov007Link *link, Ov007Follower *from, Ov007Follower *to) {
    link->from = from;
    link->to = to;
    if (from->target == 0)
        return;
    if (to->target == 0)
        return;
    VEC_Subtract(link->from->target, link->to->target, &link->dir);
    link->length = VEC_Mag(&link->dir);
}
}
