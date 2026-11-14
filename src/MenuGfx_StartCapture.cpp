//cpp
#include "nitro/hw/registers.h"

void MenuGfx_StartCapture(void) {
    REG_DISPCAPCNT = 0x80370010;
}
