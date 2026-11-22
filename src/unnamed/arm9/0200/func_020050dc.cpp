//cpp
#include "dCamera_c.h"

/* Camera state init: switch to mode record 0x1e, place lookAt and pos from
 * it (func_0200cce4), and set mFlags 0x100. */
int dCamera_c::func_020050dc() {
    this->SetMode(0x1e);
    this->func_0200cce4();
    this->mFlags |= 0x100;
    return 1;
}
