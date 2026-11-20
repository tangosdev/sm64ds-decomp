#include "nitro/spi/pm.h"

extern unsigned char gBacklightEnabled;
void TurnBacklightOn(void) {
    if (PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_ON) == PM_SUCCESS) {
        gBacklightEnabled = 1;
    }
}
