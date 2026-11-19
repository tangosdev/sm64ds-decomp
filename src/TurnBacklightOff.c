#include "nitro/spi/pm.h"

extern unsigned char gBacklightEnabled;
void TurnBacklightOff(void) {
    if (PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF) == PM_SUCCESS) {
        gBacklightEnabled = 0;
    }
}
