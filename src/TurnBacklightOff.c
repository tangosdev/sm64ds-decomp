#include "nitro/spi/pm.h"

extern unsigned char data_0208ee3c;
void TurnBacklightOff(void) {
    if (PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF) == PM_SUCCESS) {
        data_0208ee3c = 0;
    }
}
