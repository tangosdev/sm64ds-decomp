#include "Ov007Picker.h"
#include "nitro/mi/memory.h"

/* Copies each group's live points into groupPointCopies (MIi_CpuCopy32 is
 * (src, dst)) and snapshots pos as restPos. */
void func_ov007_020c6af0(Ov007Picker *c) {
    int i;
    for (i = 0; i < c->groupCount; i++) {
        MI_CpuCopy32(c->groupPoints[i], c->groupPointCopies[i], c->groups[i]->pointCount * 0xc);
    }
    c->restPos = c->pos;
}
