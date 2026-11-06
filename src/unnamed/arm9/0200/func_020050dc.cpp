//cpp
#include "dCamera_c.h"

extern "C" {

/* Camera state init: switch to mode record 0x1e, place lookAt and pos from
 * it (func_0200cce4), and set mFlags 0x100. */
int func_020050dc(dCamera_c *cam) {
    cam->func_0200cb58(0x1e);
    cam->func_0200cce4();
    cam->mFlags |= 0x100;
    return 1;
}
}
