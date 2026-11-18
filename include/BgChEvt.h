#ifndef BGCHEVT_H
#define BGCHEVT_H

#include "dActor_c.h"

class daPly_c;

/* The mover's mesh checker is the receiver. The methods are the
   checker pointer's own calls; the type has no fields and no virtuals. */
struct BgChEvt {
    int func_ov002_020ef2a4(dActor_c *other);
    int func_ov002_020ef228(dActor_c *other);
    int func_ov002_020ef070(dActor_c *mover);
    int func_ov002_020eeeb8(dActor_c *mover);
    int func_ov002_020eee3c(dActor_c *other);
    int func_ov002_020eedc0(dActor_c *other);
    int func_ov002_020eed24(daPly_c *player);
    int func_ov002_020eeca8(dActor_c *other);
    int func_ov002_020eea84(dActor_c *player);
};

#endif
